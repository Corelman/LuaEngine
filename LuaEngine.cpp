/*
* Copyright (C) 2010 - 2015 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#include "Hooks.h"
#include "LuaEngine.h"
#include "BindingMap.h"
#include "TableMgr.h"
#include "ElunaEventMgr.h"
#include "ElunaIncludes.h"
#include "ElunaTemplate.h"
#include "ElunaUtility.h"
#include "ElunaGameObjectAI.h"
#include "ElunaCreatureAI.h"
#include "ElunaInstanceAI.h"
#include "Database/DatabaseEnv.h"

#ifdef USING_BOOST
#include <boost/filesystem.hpp>
#else
#include <ace/ACE.h>
#include <ace/Dirent.h>
#include <ace/OS_NS_sys_stat.h>
#endif

extern "C"
{
// Base lua libraries
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lstate.h"

// Additional lua libraries
};

Eluna::ScriptList Eluna::lua_scripts;
Eluna::ScriptList Eluna::lua_extensions;
std::string Eluna::lua_folderpath;
std::string Eluna::lua_requirepath;
Eluna::InstanceHolder Eluna::instances;
Eluna* Eluna::GEluna(nullptr);
MsgQueue Eluna::msgque;
std::thread::id Eluna::main_thread_id(std::this_thread::get_id());
std::atomic<bool> Eluna::reload(false);
std::atomic<bool> Eluna::initialized(false);
Eluna::LockType Eluna::lock;

extern void RegisterFunctions(Eluna* E);

Eluna* Eluna::GetGEluna(const char* info)
{
    if (main_thread_id != std::this_thread::get_id())
    {
        if (info)
        {
            ELUNA_LOG_ERROR("[Eluna]: Race condition accessing GEluna. Report to devs with this message and details about what you were doing - Info: %s", info);
        }
        else
        {
            ELUNA_LOG_ERROR("[Eluna]: Race condition accessing GEluna. Report to devs with this message and details about what you were doing");
        }
        Eluna::ASSERT_MAIN_THREAD();
    }
    return Eluna::GEluna;
}

void Eluna::Initialize()
{
    ASSERT_MAIN_THREAD();
    ASSERT(!IsInitialized());

#ifdef TRINITY
    // For instance data the data column needs to be able to hold more than 255 characters (tinytext)
    // so we change it to TEXT automatically on startup
    CharacterDatabase.DirectExecute("ALTER TABLE `instance` CHANGE COLUMN `data` `data` TEXT NOT NULL");
#endif

    LoadScriptPaths();

    // Must be before creating GEluna
    // This is checked on Eluna creation
    initialized = true;

    // Create global eluna
    GEluna = new Eluna(nullptr);
    GEluna->RunScripts();
    GEluna->LoadAllSQLScripts();
}

void Eluna::Uninitialize()
{
    ASSERT_MAIN_THREAD();
    ASSERT(IsInitialized());

    delete GEluna;
    GEluna = nullptr;

    initialized = false;

    lua_scripts.clear();
    lua_extensions.clear();
}

void Eluna::LoadScriptPaths()
{
    uint32 oldMSTime = ElunaUtil::GetCurrTime();

    lua_scripts.clear();
    lua_extensions.clear();

    lua_folderpath = eConfigMgr->GetStringDefault("Eluna.ScriptPath", "lua_scripts");
#if PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_APPLE
    if (lua_folderpath[0] == '~')
        if (const char* home = getenv("HOME"))
            lua_folderpath.replace(0, 1, home);
#endif
    ELUNA_LOG_INFO("[Eluna]: Searching scripts from `%s`", lua_folderpath.c_str());
    lua_requirepath.clear();
    GetScripts(lua_folderpath);
    // Erase last ;
    if (!lua_requirepath.empty())
        lua_requirepath.erase(lua_requirepath.end() - 1);

    ELUNA_LOG_DEBUG("[Eluna]: Loaded %u scripts in %u ms", uint32(lua_scripts.size() + lua_extensions.size()), ElunaUtil::GetTimeDiff(oldMSTime));
}

#ifdef TRINITY
class ElunaAIUpdateWorker
{
public:
    void Visit(std::unordered_map<ObjectGuid, Creature*>& creatureMap)
    {
        for (auto const& p : creatureMap)
            if (p.second->IsInWorld())
                p.second->AIM_Initialize();
    }

    template<class T>
    void Visit(std::unordered_map<ObjectGuid, T*>&) { }
};
#endif

void Eluna::__ReloadEluna()
{
    // Close lua
    CloseLua();

    // remove timed events
    delete eventMgr;
    eventMgr = nullptr;

    // remove saved tables
    delete tableMgr;
    tableMgr = nullptr;

    // Open new lua and libaraies
    OpenLua();

    // new timed event mgr
    eventMgr = new EventMgr(this);

    // new table mgr
    tableMgr = new TableMgr(this);

    // Run scripts from loaded paths
    RunScripts();
}

void Eluna::_ReloadEluna()
{
    Eluna::ASSERT_MAIN_THREAD();
    ASSERT(IsInitialized());

    eWorld->SendServerMessage(SERVER_MSG_STRING, "Reloading Eluna...");

    // Reload script paths
    LoadScriptPaths();

    for (auto& e : instances.GetInstances())
        e->__ReloadEluna();

#ifdef TRINITY
    // Re initialize creature AI restoring C++ AI or applying lua AI
    sMapMgr->DoForAllMaps([](Map* map)
    {
        ElunaAIUpdateWorker worker;
        TypeContainerVisitor<ElunaAIUpdateWorker, MapStoredObjectTypesContainer> visitor(worker);
        visitor.Visit(map->GetObjectsStore());
    });
#endif

    reload = false;
}

Eluna::Eluna(Map* map) :
event_level(0),
push_counter(0),
enabled(false),
safe_mode(false),

current_thread_id(std::this_thread::get_id()),
eventMgr(nullptr),
owner(map),
L(nullptr),

ServerEventBindings(nullptr),
PlayerEventBindings(nullptr),
GuildEventBindings(nullptr),
GroupEventBindings(nullptr),
VehicleEventBindings(nullptr),
BGEventBindings(nullptr),

CreatureEventBindings(nullptr),
CreatureGossipBindings(nullptr),
GameObjectEventBindings(nullptr),
GameObjectGossipBindings(nullptr),
ItemEventBindings(nullptr),
ItemGossipBindings(nullptr),
PlayerGossipBindings(nullptr),
MapEventBindings(nullptr),
InstanceEventBindings(nullptr),

CreatureUniqueBindings(nullptr)
{
    ASSERT(IsInitialized());

    OpenLua();

    // Replace this with map insert if making multithread version

    // Set event manager
    eventMgr = new EventMgr(this);
    tableMgr = new TableMgr(this);

    instances.Add(this);
}

Eluna::~Eluna()
{
    ASSERT(IsInitialized());

    instances.Remove(this);

    CloseLua();

    delete eventMgr;
    eventMgr = nullptr;

    delete tableMgr;
    tableMgr = nullptr;
}

void Eluna::CloseLua()
{
    OnLuaStateClose();

    DestroyBindStores();

    // Must close lua state after deleting stores and mgr
    if (L)
        lua_close(L);
    L = nullptr;

    instanceDataRefs.clear();
    continentDataRefs.clear();
}

void Eluna::OpenLua()
{
    enabled = eConfigMgr->GetBoolDefault("Eluna.Enabled", true);
    if (!IsEnabled())
    {
        ELUNA_LOG_INFO("[Eluna]: Eluna is disabled in config");
        return;
    }

    L = luaL_newstate();

    lua_pushlightuserdata(L, this);
    lua_setglobal(L, ELUNA_STATE_PTR);

    CreateBindStores();

    // open base lua libraries
    // -- to make states cheaper to create, comment this out and use require("table") etc to get libs you use
    luaL_openlibs(L);

    // open additional lua libraries

    // Register methods and functions
    RegisterFunctions(this);

    // Create hidden table with weak values
    lua_newtable(L);
    lua_newtable(L);
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2);
    lua_setglobal(L, ELUNA_OBJECT_STORE);

    // Set lua require folder paths (scripts folder structure)
    lua_getglobal(L, "package");
    lua_pushstring(L, lua_requirepath.c_str());
    lua_setfield(L, -2, "path");
    lua_pushstring(L, ""); // erase cpath
    lua_setfield(L, -2, "cpath");
    lua_pop(L, 1);

    ASSERT(!lua_gettop(L)); // Stack should be empty
}

void Eluna::CreateBindStores()
{
    DestroyBindStores();

    ServerEventBindings      = new BindingMap< EventKey<Hooks::ServerEvents> >(L);
    PlayerEventBindings      = new BindingMap< EventKey<Hooks::PlayerEvents> >(L);
    GuildEventBindings       = new BindingMap< EventKey<Hooks::GuildEvents> >(L);
    GroupEventBindings       = new BindingMap< EventKey<Hooks::GroupEvents> >(L);
    VehicleEventBindings     = new BindingMap< EventKey<Hooks::VehicleEvents> >(L);
    BGEventBindings          = new BindingMap< EventKey<Hooks::BGEvents> >(L);

    CreatureEventBindings    = new BindingMap< EntryKey<Hooks::CreatureEvents> >(L);
    CreatureGossipBindings   = new BindingMap< EntryKey<Hooks::GossipEvents> >(L);
    GameObjectEventBindings  = new BindingMap< EntryKey<Hooks::GameObjectEvents> >(L);
    GameObjectGossipBindings = new BindingMap< EntryKey<Hooks::GossipEvents> >(L);
    ItemEventBindings        = new BindingMap< EntryKey<Hooks::ItemEvents> >(L);
    ItemGossipBindings       = new BindingMap< EntryKey<Hooks::GossipEvents> >(L);
    PlayerGossipBindings     = new BindingMap< EntryKey<Hooks::GossipEvents> >(L);
    MapEventBindings         = new BindingMap< EntryKey<Hooks::InstanceEvents> >(L);
    InstanceEventBindings    = new BindingMap< EntryKey<Hooks::InstanceEvents> >(L);

    CreatureUniqueBindings   = new BindingMap< UniqueObjectKey<Hooks::CreatureEvents> >(L);
}

void Eluna::DestroyBindStores()
{
    delete ServerEventBindings;
    delete PlayerEventBindings;
    delete GuildEventBindings;
    delete GroupEventBindings;
    delete VehicleEventBindings;

    delete CreatureEventBindings;
    delete CreatureGossipBindings;
    delete GameObjectEventBindings;
    delete GameObjectGossipBindings;
    delete ItemEventBindings;
    delete ItemGossipBindings;
    delete PlayerGossipBindings;
    delete BGEventBindings;
    delete MapEventBindings;
    delete InstanceEventBindings;

    delete CreatureUniqueBindings;

    ServerEventBindings = nullptr;
    PlayerEventBindings = nullptr;
    GuildEventBindings = nullptr;
    GroupEventBindings = nullptr;
    VehicleEventBindings = nullptr;

    CreatureEventBindings = nullptr;
    CreatureGossipBindings = nullptr;
    GameObjectEventBindings = nullptr;
    GameObjectGossipBindings = nullptr;
    ItemEventBindings = nullptr;
    ItemGossipBindings = nullptr;
    PlayerGossipBindings = nullptr;
    BGEventBindings = nullptr;
    MapEventBindings = nullptr;
    InstanceEventBindings = nullptr;

    CreatureUniqueBindings = nullptr;
}

std::unordered_map<uint32, std::string> Eluna::luaScriptsSQL;
std::unordered_map<uint32, std::unordered_set<uint32>> Eluna::luaScriptMapping;

void Eluna::AddScriptPath(std::string filename, const std::string& fullpath)
{
    ELUNA_LOG_DEBUG("[Eluna]: AddScriptPath Checking file `%s`", fullpath.c_str());

    // split file name
    std::size_t extDot = filename.find_last_of('.');
    if (extDot == std::string::npos)
        return;
    std::string ext = filename.substr(extDot);
    filename = filename.substr(0, extDot);

    // check extension and add path to scripts to load
    if (ext != ".lua" && ext != ".dll" && ext != ".ext")
        return;
    bool extension = ext == ".ext";

    LuaScript script;
    script.fileext = ext;
    script.filename = filename;
    script.filepath = fullpath;
    script.modulepath = fullpath.substr(0, fullpath.length() - filename.length() - ext.length());
    if (extension)
        lua_extensions.push_back(script);
    else
        lua_scripts.push_back(script);
    ELUNA_LOG_DEBUG("[Eluna]: AddScriptPath add path `%s`", fullpath.c_str());
}

// Finds lua script files from given path (including subdirectories) and pushes them to scripts
void Eluna::GetScripts(std::string path)
{
    ELUNA_LOG_DEBUG("[Eluna]: GetScripts from path `%s`", path.c_str());

#ifdef USING_BOOST
    boost::filesystem::path someDir(path);
    boost::filesystem::directory_iterator end_iter;

    if (boost::filesystem::exists(someDir) && boost::filesystem::is_directory(someDir))
    {
        lua_requirepath +=
            path + "/?;" +
            path + "/?.lua;" +
            path + "/?.ext;" +
            path + "/?.dll;" +
            path + "/?.so;";

        for (boost::filesystem::directory_iterator dir_iter(someDir); dir_iter != end_iter; ++dir_iter)
        {
            std::string fullpath = dir_iter->path().generic_string();

            // Check if file is hidden
#ifdef WIN32
            DWORD dwAttrib = GetFileAttributes(fullpath.c_str());
            if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_HIDDEN))
                continue;
#endif
#ifdef UNIX
            std::string name = dir_iter->path().filename().generic_string().c_str();
            if (name != ".." || name != "." || name[0] == '.')
                continue;
#endif

            // load subfolder
            if (boost::filesystem::is_directory(dir_iter->status()))
            {
                GetScripts(fullpath);
                continue;
            }

            if (boost::filesystem::is_regular_file(dir_iter->status()))
            {
                // was file, try add
                std::string filename = dir_iter->path().filename().generic_string();
                AddScriptPath(filename, fullpath);
            }
        }
    }
#else
    ACE_Dirent dir;
    if (dir.open(path.c_str()) == -1) // Error opening directory, return
        return;

    lua_requirepath +=
        path + "/?;" +
        path + "/?.lua;" +
        path + "/?.ext;" +
        path + "/?.dll;" +
        path + "/?.so;";

    ACE_DIRENT *directory = 0;
    while ((directory = dir.read()))
    {
        // Skip the ".." and "." files.
        if (ACE::isdotdir(directory->d_name))
            continue;

        std::string fullpath = path + "/" + directory->d_name;

        // Check if file is hidden
#ifdef WIN32
        DWORD dwAttrib = GetFileAttributes(fullpath.c_str());
        if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_HIDDEN))
            continue;
#endif
#ifdef UNIX
        std::string name = directory->d_name.c_str();
        if (name != ".." || name != "." || name[0] == '.')
            continue;
#endif

        ACE_stat stat_buf;
        if (ACE_OS::lstat(fullpath.c_str(), &stat_buf) == -1)
            continue;

        // load subfolder
        if ((stat_buf.st_mode & S_IFMT) == (S_IFDIR))
        {
            GetScripts(fullpath);
            continue;
        }

        // was file, try add
        std::string filename = directory->d_name;
        AddScriptPath(filename, fullpath);
    }
#endif
}

static bool ScriptPathComparator(const LuaScript& first, const LuaScript& second)
{
    return first.filepath < second.filepath;
}


void Eluna::LoadAllSQLScripts() 
{
    // Load all Scripts
    QueryResult *result = WorldDatabase.PQuery("SELECT id, script FROM lua_scripts WHERE status = 'Enabled'");
    int counter = 0;
    if(result) 
    {
        do 
        {
            Field* fields = result->Fetch();
            uint32 scriptId = fields[0].GetUInt32();
            std::string content = fields[1].GetCppString();
            Eluna::luaScriptsSQL[scriptId] = content;
            counter++;
	}
        while(result->NextRow());
		
        delete(result);
        ELUNA_LOG_DEBUG("[Eluna]: Loaded `%u` Lua Scripts from SQL in memory.", counter);
    }
    
    counter = 0;
	
    //fill mappings
    result = WorldDatabase.PQuery("SELECT map_id, script_id FROM lua_map_scripts ORDER BY map_id");
    if(result) 
    {
        do 
        {
	    Field* fields = result->Fetch();
			
            uint32 mapId = fields[0].GetUInt32();
            uint32 scriptId = fields[1].GetUInt32();
			
            auto search = Eluna::luaScriptMapping.find(mapId);
			
            Eluna::luaScriptMapping[mapId].insert(scriptId);
            counter++;
			
        }
        while(result->NextRow());
		
        delete(result);
        ELUNA_LOG_DEBUG("[Eluna]: Loaded `%u` Lua Mappings from SQL in memory.", counter);
    }
}

void Eluna::RunSQLMapScripts(uint32 mapId)
{
    ELUNA_LOG_DEBUG("[Eluna]: Looking for Cached LUA Scripts in Map: %u", mapId);
    auto search = Eluna::luaScriptMapping.find(mapId);
    if (search != Eluna::luaScriptMapping.end()) 
    {
        for (auto itr = Eluna::luaScriptMapping[mapId].begin(); itr != Eluna::luaScriptMapping[mapId].end(); ++itr) 
        {
            RunSQLScript(*itr);
        }
		
    }
}

void Eluna::RunSQLScript(uint32 scriptId) 
{
    if (!IsEnabled())
        return;
	
    char numstr[21]; // enough to hold all numbers up to 64-bits

    std::string scriptPrefix = "SQLScript_";
    std::string scriptName = scriptPrefix + ACE_OS::itoa(scriptId, numstr, 10);
    std::string scriptContent = "";
	
    auto search = Eluna::luaScriptsSQL.find(scriptId);
    if (search != Eluna::luaScriptsSQL.end())
    {
        scriptContent = Eluna::luaScriptsSQL[scriptId];
    }
    else
    {
        ELUNA_LOG_ERROR("[Eluna]: Error loading `%s`. LUA Script with ID `%u` could not be found in memory.", scriptName.c_str(), scriptId);
        return;
    }
	
    int base = lua_gettop(L);
	
    uint32 oldMSTime = ElunaUtil::GetCurrTime();
    std::unordered_map<std::string, std::string> loaded; // filename, path
    lua_getglobal(L, "package");
    // Stack: package
    luaL_getsubtable(L, -1, "loaded");
    // Stack: package, modules
    int modules = lua_gettop(L);
    
    // Check that no duplicate names exist
    if (loaded.find(scriptName) != loaded.end())
    {
        ELUNA_LOG_ERROR("[Eluna]: Error loading `%s`. File with same name already loaded from `%s`, rename either file", scriptName.c_str(), loaded[scriptName].c_str());
        lua_pop(L, 2);
        return;
    }
    loaded[scriptName] = scriptName;
    
    lua_getfield(L, modules, scriptName.c_str());
    // Stack: package, modules, module
    if (!lua_isnoneornil(L, -1))
    {
        lua_pop(L, 1);
        ELUNA_LOG_DEBUG("[Eluna]: `%s` was already loaded or required", scriptName.c_str());
        lua_pop(L, 2);
        return;
    }
    lua_pop(L, 1);
	
    LuaStringScriptLoader loader(scriptName.c_str(), scriptContent);
    if (RunScript(loader))
    {
        // Stack: package, modules, result
        lua_setfield(L, modules, scriptName.c_str()); // sets modules[$filename] = callresult
        // Stack: package, modules
        ASSERT(lua_gettop(L) == base + 2);
        ELUNA_LOG_DEBUG("[Eluna]: Successfully loaded `%s`", scriptName.c_str());
    } 
    else
    {
        lua_pop(L, 1);
        ASSERT(lua_gettop(L) == base + 2);
    }
    
    // Stack: package, modules
    lua_pop(L, 2);
    OnLuaStateOpen();
	
}

void Eluna::RunScripts()
{
    if (!IsEnabled())
        return;

    uint32 oldMSTime = ElunaUtil::GetCurrTime();
    uint32 count = 0;

    ScriptList scripts;
    lua_extensions.sort(ScriptPathComparator);
    lua_scripts.sort(ScriptPathComparator);
    scripts.insert(scripts.end(), lua_extensions.begin(), lua_extensions.end());
    scripts.insert(scripts.end(), lua_scripts.begin(), lua_scripts.end());

    std::unordered_map<std::string, std::string> loaded; // filename, path
    count += RunScripts(lua_extensions, loaded);

    // Setup safe_mode here
    safe_mode = eConfigMgr->GetBoolDefault("Eluna.SafeMode", true);
    lua_getglobal(L, "sandbox_env");
    if (lua_istable(L, -1))
        lua_setglobal(L, ELUNA_SAFE_MODE_ENV);
    else
    {
        lua_pop(L, 1);
        if (safe_mode)
            ELUNA_LOG_ERROR("LUA variable sandbox_env not found ! LUA scripts won't load.");
        lua_newtable(L);
        lua_setglobal(L, ELUNA_SAFE_MODE_ENV);
    }
    ASSERT(lua_gettop(L) == 0); // Stack should be empty

    count += RunScripts(lua_scripts, loaded);

    ELUNA_LOG_DEBUG("[Eluna]: Executed %u Lua scripts (including %zu extensions) in %u ms", count, lua_extensions.size(), ElunaUtil::GetTimeDiff(oldMSTime));

    OnLuaStateOpen();
}

uint32 Eluna::RunScripts(ScriptList const& scripts, std::unordered_map<std::string, std::string>& loaded)
{
    uint32 count = 0;
    int base = lua_gettop(L);
    lua_getglobal(L, "package");
    // Stack: package
    luaL_getsubtable(L, -1, "loaded");
    // Stack: package, modules
    int modules = lua_gettop(L);
    for (ScriptList::const_iterator it = scripts.begin(); it != scripts.end(); ++it)
    {
        // Check that no duplicate names exist
        if (loaded.find(it->filename) != loaded.end())
        {
            ELUNA_LOG_ERROR("[Eluna]: Error loading `%s`. File with same name already loaded from `%s`, rename either file", it->filepath.c_str(), loaded[it->filename].c_str());
            continue;
        }
        loaded[it->filename] = it->filepath;

        lua_getfield(L, modules, it->filename.c_str());
        // Stack: package, modules, module
        if (!lua_isnoneornil(L, -1))
        {
            lua_pop(L, 1);
            ELUNA_LOG_DEBUG("[Eluna]: `%s` was already loaded or required", it->filepath.c_str());
            continue;
        }
        lua_pop(L, 1);
        LuaFileScriptLoader loader(it->filename.c_str(), it->filepath.c_str());
        if (RunScript(loader))
        {
            // Stack: package, modules, result
            lua_setfield(L, modules, it->filename.c_str()); // sets modules[$filename] = callresult
            // Stack: package, modules
            ASSERT(lua_gettop(L) == base + 2);
            ELUNA_LOG_DEBUG("[Eluna]: Successfully loaded `%s`", it->filepath.c_str());
            ++count;
            continue;
        }
        lua_pop(L, 1);
        ASSERT(lua_gettop(L) == base + 2);
    }
    // Stack: package, modules
    lua_pop(L, 2);
    return count;
}

const char *Eluna_RunScript_Reader(lua_State *,
                                    void *data,
                                    size_t *size)
{
    LuaScriptLoader* script = (LuaScriptLoader*)data;
    return script->Read(*size);
}

bool Eluna::RunScript(LuaScriptLoader& loader)
{
    // Allow binary and text modes ?
    const char* mode = safe_mode ? "t" : "bt"; // Possible flaws with binary code
    int loadResult = lua_load(L, Eluna_RunScript_Reader, (&loader), loader.GetScriptName(), "bt");
    if (loadResult) // Syntax error
    {
        // Stack: errmsg
        ELUNA_LOG_ERROR("[Eluna]: Error %i loading `%s`", loadResult, loader.GetScriptName());
        Report(L);
        Push(L, false); // Call failed
        return false;
    }
    // Stack: filefunc

    // Sandbox protection - before script execution
    if (safe_mode)
        EnableSandboxForCall(L);

    if (ExecuteCall(0, 1))
    {
        // Stack: result
        if (lua_isnoneornil(L, -1) || (lua_isboolean(L, -1) && !lua_toboolean(L, -1)))
        {
            // if result evaluates to false, change it to true
            lua_pop(L, 1);
            Push(L, true);
        }
        // Stack: result

        // successfully loaded and ran script
        return true;
    }
    return false;
}

void Eluna::InvalidateObjects()
{
    lua_getglobal(L, ELUNA_OBJECT_STORE);
    ASSERT(lua_istable(L, -1));

    lua_pushnil(L);
    while (lua_next(L, -2))
    {
        if (ElunaObject* elunaObj = CHECKOBJ<ElunaObject>(L, -1, false))
            elunaObj->Invalidate();
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

void Eluna::Report(lua_State* _L)
{
    const char* msg = lua_tostring(_L, -1);
    ELUNA_LOG_ERROR("%s", msg);
    lua_pop(_L, 1);
}

// Borrowed from http://stackoverflow.com/questions/12256455/print-stacktrace-from-c-code-with-embedded-lua
int Eluna::StackTrace(lua_State *_L)
{
    // Stack: errmsg
    if (!lua_isstring(_L, -1))  /* 'message' not a string? */
        return 1;  /* keep it intact */
    // Stack: errmsg, debug
    lua_getglobal(_L, "debug");
    if (!lua_istable(_L, -1))
    {
        lua_pop(_L, 1);
        return 1;
    }
    // Stack: errmsg, debug, traceback
    lua_getfield(_L, -1, "traceback");
    if (!lua_isfunction(_L, -1))
    {
        lua_pop(_L, 2);
        return 1;
    }
    lua_pushvalue(_L, -3);  /* pass error message */
    lua_pushinteger(_L, 1);  /* skip this function and traceback */
    // Stack: errmsg, debug, traceback, errmsg, 2
    lua_call(_L, 2, 1);  /* call debug.traceback */

    // dirty stack?
    // Stack: errmsg, debug, tracemsg
    return 1;
}

bool Eluna::ExecuteCall(int params, int res)
{
    int top = lua_gettop(L);
    int base = top - params;

    // Expected: function, [parameters]
    ASSERT(base > 0);

    // Check function type
    if (!lua_isfunction(L, base))
    {
        ELUNA_LOG_ERROR("[Eluna]: Cannot execute call: registered value is %s, not a function.", luaL_tolstring(L, base, nullptr));
        ASSERT(false); // stack probably corrupt
    }

    bool usetrace = eConfigMgr->GetBoolDefault("Eluna.TraceBack", false);
    if (usetrace)
    {
        lua_pushcfunction(L, &StackTrace);
        // Stack: function, [parameters], traceback
        lua_insert(L, base);
        // Stack: traceback, function, [parameters]
    }

    // Objects are invalidated when event_level hits 0
    ++event_level;
    int result = lua_pcall(L, params, res, usetrace ? base : 0);
    --event_level;

    if (usetrace)
    {
        // Stack: traceback, [results or errmsg]
        lua_remove(L, base);
    }
    // Stack: [results or errmsg]

    // lua_pcall returns 0 on success.
    // On error print the error and push nils for expected amount of returned values
    if (result)
    {
        // Stack: errmsg
        Report(L);

        // Force garbage collect
        lua_gc(L, LUA_GCCOLLECT, 0);

        // Push nils for expected amount of results
        for (int i = 0; i < res; ++i)
            lua_pushnil(L);
        // Stack: [nils]
        return false;
    }

    // Stack: [results]
    return true;
}

void Eluna::Push(lua_State* luastate)
{
    lua_pushnil(luastate);
}
void Eluna::Push(lua_State* luastate, const uint64 l)
{
    Push(luastate, static_cast<int64>(l));
}
void Eluna::Push(lua_State* luastate, const int8 i)
{
    lua_pushinteger(luastate, i);
}
void Eluna::Push(lua_State* luastate, const uint8 i)
{
    lua_pushinteger(luastate, i);
}
void Eluna::Push(lua_State* luastate, const int16 i)
{
    lua_pushinteger(luastate, i);
}
void Eluna::Push(lua_State* luastate, const uint16 i)
{
    lua_pushinteger(luastate, i);
}
void Eluna::Push(lua_State* luastate, const int32 i)
{
    lua_pushinteger(luastate, i);
}
void Eluna::Push(lua_State* luastate, const uint32 i)
{
    lua_pushinteger(luastate, i);
}
void Eluna::Push(lua_State* luastate, const int64 i)
{
    lua_pushinteger(luastate, i);
}
void Eluna::Push(lua_State* luastate, const double d)
{
    lua_pushnumber(luastate, d);
}
void Eluna::Push(lua_State* luastate, const float f)
{
    lua_pushnumber(luastate, f);
}
void Eluna::Push(lua_State* luastate, const bool b)
{
    lua_pushboolean(luastate, b);
}
void Eluna::Push(lua_State* luastate, const std::string& str)
{
    lua_pushstring(luastate, str.c_str());
}
void Eluna::Push(lua_State* luastate, const char* str)
{
    lua_pushstring(luastate, str);
}
void Eluna::Push(lua_State* luastate, Pet const* pet)
{
    Push<Creature>(luastate, pet);
}
void Eluna::Push(lua_State* luastate, TempSummon const* summon)
{
    Push<Creature>(luastate, summon);
}
void Eluna::Push(lua_State* luastate, Unit const* unit)
{
    if (!unit)
    {
        Push(luastate);
        return;
    }
    switch (unit->GetTypeId())
    {
        case TYPEID_UNIT:
            Push(luastate, unit->ToCreature());
            break;
        case TYPEID_PLAYER:
            Push(luastate, unit->ToPlayer());
            break;
        default:
            ElunaTemplate<Unit>::Push(luastate, unit);
    }
}
void Eluna::Push(lua_State* luastate, WorldObject const* obj)
{
    if (!obj)
    {
        Push(luastate);
        return;
    }
    switch (obj->GetTypeId())
    {
        case TYPEID_UNIT:
            Push(luastate, obj->ToCreature());
            break;
        case TYPEID_PLAYER:
            Push(luastate, obj->ToPlayer());
            break;
        case TYPEID_GAMEOBJECT:
            Push(luastate, obj->ToGameObject());
            break;
        case TYPEID_CORPSE:
            Push(luastate, obj->ToCorpse());
            break;
        default:
            ElunaTemplate<WorldObject>::Push(luastate, obj);
    }
}
void Eluna::Push(lua_State* luastate, Object const* obj)
{
    if (!obj)
    {
        Push(luastate);
        return;
    }
    switch (obj->GetTypeId())
    {
        case TYPEID_UNIT:
            Push(luastate, obj->ToCreature());
            break;
        case TYPEID_PLAYER:
            Push(luastate, obj->ToPlayer());
            break;
        case TYPEID_GAMEOBJECT:
            Push(luastate, obj->ToGameObject());
            break;
        case TYPEID_CORPSE:
            Push(luastate, obj->ToCorpse());
            break;
        default:
            ElunaTemplate<Object>::Push(luastate, obj);
    }
}

template<typename T>
static T CheckIntegerRange(lua_State* luastate, int narg)
{
    lua_Integer value = luaL_checkinteger(luastate, narg);

    if (value > static_cast<lua_Integer>(std::numeric_limits<T>::max()))
    {
        std::ostringstream ss;
        ss << "value must be less than or equal to " << static_cast<lua_Integer>(std::numeric_limits<T>::max());
        return luaL_argerror(luastate, narg, ss.str().c_str());
    }

    if (value < static_cast<lua_Integer>(std::numeric_limits<T>::min()))
    {
        std::ostringstream ss;
        ss << "value must be greater than or equal to " << static_cast<lua_Integer>(std::numeric_limits<T>::min());
        return luaL_argerror(luastate, narg, ss.str().c_str());
    }
    return value;
}

template<> bool Eluna::CHECKVAL<bool>(lua_State* luastate, int narg)
{
    return lua_toboolean(luastate, narg) != 0;
}
template<> float Eluna::CHECKVAL<float>(lua_State* luastate, int narg)
{
    return static_cast<float>(luaL_checknumber(luastate, narg));
}
template<> double Eluna::CHECKVAL<double>(lua_State* luastate, int narg)
{
    return luaL_checknumber(luastate, narg);
}
template<> int8 Eluna::CHECKVAL<int8>(lua_State* luastate, int narg)
{
    return CheckIntegerRange<int8>(luastate, narg);
}
template<> uint8 Eluna::CHECKVAL<uint8>(lua_State* luastate, int narg)
{
    return CheckIntegerRange<uint8>(luastate, narg);
}
template<> int16 Eluna::CHECKVAL<int16>(lua_State* luastate, int narg)
{
    return CheckIntegerRange<int16>(luastate, narg);
}
template<> uint16 Eluna::CHECKVAL<uint16>(lua_State* luastate, int narg)
{
    return CheckIntegerRange<uint16>(luastate, narg);
}
template<> int32 Eluna::CHECKVAL<int32>(lua_State* luastate, int narg)
{
    return CheckIntegerRange<int32>(luastate, narg);
}
template<> uint32 Eluna::CHECKVAL<uint32>(lua_State* luastate, int narg)
{
    return CheckIntegerRange<uint32>(luastate, narg);
}
template<> int64 Eluna::CHECKVAL<int64>(lua_State* luastate, int narg)
{
    return CheckIntegerRange<int64>(luastate, narg);
}
template<> uint64 Eluna::CHECKVAL<uint64>(lua_State* luastate, int narg)
{
    return static_cast<uint64>(CHECKVAL<int64>(luastate, narg));
}
template<> const char* Eluna::CHECKVAL<const char*>(lua_State* luastate, int narg)
{
    return luaL_checkstring(luastate, narg);
}
template<> std::string Eluna::CHECKVAL<std::string>(lua_State* luastate, int narg)
{
    return luaL_checkstring(luastate, narg);
}

template<> Object* Eluna::CHECKOBJ<Object>(lua_State* luastate, int narg, bool error)
{
    Object* obj = CHECKOBJ<WorldObject>(luastate, narg, false);
    if (!obj)
        obj = CHECKOBJ<Item>(luastate, narg, false);
    if (!obj)
        obj = ElunaTemplate<Object>::Check(luastate, narg, error);
    return obj;
}
template<> WorldObject* Eluna::CHECKOBJ<WorldObject>(lua_State* luastate, int narg, bool error)
{
    WorldObject* obj = CHECKOBJ<Unit>(luastate, narg, false);
    if (!obj)
        obj = CHECKOBJ<GameObject>(luastate, narg, false);
    if (!obj)
        obj = CHECKOBJ<Corpse>(luastate, narg, false);
    if (!obj)
        obj = ElunaTemplate<WorldObject>::Check(luastate, narg, error);
    return obj;
}
template<> Unit* Eluna::CHECKOBJ<Unit>(lua_State* luastate, int narg, bool error)
{
    Unit* obj = CHECKOBJ<Player>(luastate, narg, false);
    if (!obj)
        obj = CHECKOBJ<Creature>(luastate, narg, false);
    if (!obj)
        obj = ElunaTemplate<Unit>::Check(luastate, narg, error);
    return obj;
}

template<> ElunaObject* Eluna::CHECKOBJ<ElunaObject>(lua_State* luastate, int narg, bool error)
{
    return CHECKTYPE(luastate, narg, nullptr, error);
}

ElunaObject* Eluna::CHECKTYPE(lua_State* luastate, int narg, const char* tname, bool error)
{
    bool valid = false;
    ElunaObject** ptrHold = nullptr;

    if (!tname)
    {
        valid = true;
        ptrHold = static_cast<ElunaObject**>(lua_touserdata(luastate, narg));
    }
    else
    {
        if (lua_getmetatable(luastate, narg))
        {
            lua_getglobal(luastate, tname);
            bool equal = lua_rawequal(luastate, -1, -2) == 1;
            lua_pop(luastate, 2);
            if (equal)
            {
                valid = true;
                ptrHold = static_cast<ElunaObject**>(lua_touserdata(luastate, narg));
            }
        }
    }

    if (!valid || !ptrHold)
    {
        if (error)
        {
            char buff[256];
            snprintf(buff, 256, "bad argument : %s expected, got %s", tname ? tname : "ElunaObject", luaL_typename(luastate, narg));
            luaL_argerror(luastate, narg, buff);
        }
        return nullptr;
    }
    return *ptrHold;
}

template<typename K>
static int cancelBinding(lua_State *L)
{
    uint64 bindingID = Eluna::CHECKVAL<uint64>(L, lua_upvalueindex(1));

    BindingMap<K>* bindings = (BindingMap<K>*)lua_touserdata(L, lua_upvalueindex(2));
    ASSERT(bindings != nullptr);

    bindings->Remove(bindingID);

    return 0;
}

template<typename K>
static void createCancelCallback(lua_State* L, uint64 bindingID, BindingMap<K>* bindings)
{
    Eluna::Push(L, bindingID);
    lua_pushlightuserdata(L, bindings);
    // Stack: bindingID, bindings

    lua_pushcclosure(L, &cancelBinding<K>, 2);
    // Stack: cancel_callback
}

// Saves the function reference ID given to the register type's store for given entry under the given event
int Eluna::Register(lua_State* L, uint8 regtype, uint32 entry, uint64 guid, uint32 instanceId, uint32 event_id, int functionRef, uint32 shots)
{
    uint64 bindingID;

    switch (regtype)
    {
        case Hooks::REGTYPE_SERVER:
            if (event_id < Hooks::SERVER_EVENT_COUNT)
            {
                auto key = EventKey<Hooks::ServerEvents>((Hooks::ServerEvents)event_id);
                bindingID = ServerEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, ServerEventBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_PLAYER:
            if (event_id < Hooks::PLAYER_EVENT_COUNT)
            {
                auto key = EventKey<Hooks::PlayerEvents>((Hooks::PlayerEvents)event_id);
                bindingID = PlayerEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, PlayerEventBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_GUILD:
            if (event_id < Hooks::GUILD_EVENT_COUNT)
            {
                auto key = EventKey<Hooks::GuildEvents>((Hooks::GuildEvents)event_id);
                bindingID = GuildEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, GuildEventBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_GROUP:
            if (event_id < Hooks::GROUP_EVENT_COUNT)
            {
                auto key = EventKey<Hooks::GroupEvents>((Hooks::GroupEvents)event_id);
                bindingID = GroupEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, GroupEventBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_VEHICLE:
            if (event_id < Hooks::VEHICLE_EVENT_COUNT)
            {
                auto key = EventKey<Hooks::VehicleEvents>((Hooks::VehicleEvents)event_id);
                bindingID = VehicleEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, VehicleEventBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_BG:
            if (event_id < Hooks::BG_EVENT_COUNT)
            {
                auto key = EventKey<Hooks::BGEvents>((Hooks::BGEvents)event_id);
                bindingID = BGEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, BGEventBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_CREATURE:
            if (event_id < Hooks::CREATURE_EVENT_COUNT)
            {
                if (entry != 0)
                {
                    if (!eObjectMgr->GetCreatureTemplate(entry))
                    {
                        luaL_unref(L, LUA_REGISTRYINDEX, functionRef);
                        luaL_error(L, "Couldn't find a creature with (ID: %d)!", entry);
                        return 0; // Stack: (empty)
                    }

                    auto key = EntryKey<Hooks::CreatureEvents>((Hooks::CreatureEvents)event_id, entry);
                    bindingID = CreatureEventBindings->Insert(key, functionRef, shots);
                    createCancelCallback(L, bindingID, CreatureEventBindings);
                }
                else
                {
                    ASSERT(guid != 0);

                    auto key = UniqueObjectKey<Hooks::CreatureEvents>((Hooks::CreatureEvents)event_id, guid, instanceId);
                    bindingID = CreatureUniqueBindings->Insert(key, functionRef, shots);
                    createCancelCallback(L, bindingID, CreatureUniqueBindings);
                }
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_CREATURE_GOSSIP:
            if (event_id < Hooks::GOSSIP_EVENT_COUNT)
            {
                if (!eObjectMgr->GetCreatureTemplate(entry))
                {
                    luaL_unref(L, LUA_REGISTRYINDEX, functionRef);
                    luaL_error(L, "Couldn't find a creature with (ID: %d)!", entry);
                    return 0; // Stack: (empty)
                }

                auto key = EntryKey<Hooks::GossipEvents>((Hooks::GossipEvents)event_id, entry);
                bindingID = CreatureGossipBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, CreatureGossipBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_GAMEOBJECT:
            if (event_id < Hooks::GAMEOBJECT_EVENT_COUNT)
            {
                if (!eObjectMgr->GetGameObjectTemplate(entry))
                {
                    luaL_unref(L, LUA_REGISTRYINDEX, functionRef);
                    luaL_error(L, "Couldn't find a gameobject with (ID: %d)!", entry);
                    return 0; // Stack: (empty)
                }

                auto key = EntryKey<Hooks::GameObjectEvents>((Hooks::GameObjectEvents)event_id, entry);
                bindingID = GameObjectEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, GameObjectEventBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_GAMEOBJECT_GOSSIP:
            if (event_id < Hooks::GOSSIP_EVENT_COUNT)
            {
                if (!eObjectMgr->GetGameObjectTemplate(entry))
                {
                    luaL_unref(L, LUA_REGISTRYINDEX, functionRef);
                    luaL_error(L, "Couldn't find a gameobject with (ID: %d)!", entry);
                    return 0; // Stack: (empty)
                }

                auto key = EntryKey<Hooks::GossipEvents>((Hooks::GossipEvents)event_id, entry);
                bindingID = GameObjectGossipBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, GameObjectGossipBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_ITEM:
            if (event_id < Hooks::ITEM_EVENT_COUNT)
            {
                if (!eObjectMgr->GetItemTemplate(entry))
                {
                    luaL_unref(L, LUA_REGISTRYINDEX, functionRef);
                    luaL_error(L, "Couldn't find a item with (ID: %d)!", entry);
                    return 0; // Stack: (empty)
                }

                auto key = EntryKey<Hooks::ItemEvents>((Hooks::ItemEvents)event_id, entry);
                bindingID = ItemEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, ItemEventBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_ITEM_GOSSIP:
            if (event_id < Hooks::GOSSIP_EVENT_COUNT)
            {
                if (!eObjectMgr->GetItemTemplate(entry))
                {
                    luaL_unref(L, LUA_REGISTRYINDEX, functionRef);
                    luaL_error(L, "Couldn't find a item with (ID: %d)!", entry);
                    return 0; // Stack: (empty)
                }

                auto key = EntryKey<Hooks::GossipEvents>((Hooks::GossipEvents)event_id, entry);
                bindingID = ItemGossipBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, ItemGossipBindings);
                return 1; // Stack: callback
            }
            break;

        case Hooks::REGTYPE_PLAYER_GOSSIP:
            if (event_id < Hooks::GOSSIP_EVENT_COUNT)
            {
                auto key = EntryKey<Hooks::GossipEvents>((Hooks::GossipEvents)event_id, entry);
                bindingID = PlayerGossipBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, PlayerGossipBindings);
                return 1; // Stack: callback
            }
            break;
        case Hooks::REGTYPE_MAP:
            if (event_id < Hooks::INSTANCE_EVENT_COUNT)
            {
                auto key = EntryKey<Hooks::InstanceEvents>((Hooks::InstanceEvents)event_id, entry);
                bindingID = MapEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, MapEventBindings);
                return 1; // Stack: callback
            }
            break;
        case Hooks::REGTYPE_INSTANCE:
            if (event_id < Hooks::INSTANCE_EVENT_COUNT)
            {
                auto key = EntryKey<Hooks::InstanceEvents>((Hooks::InstanceEvents)event_id, entry);
                bindingID = InstanceEventBindings->Insert(key, functionRef, shots);
                createCancelCallback(L, bindingID, InstanceEventBindings);
                return 1; // Stack: callback
            }
            break;
    }
    luaL_unref(L, LUA_REGISTRYINDEX, functionRef);
    std::ostringstream oss;
    oss << "regtype " << static_cast<uint32>(regtype) << ", event " << event_id << ", entry " << entry << ", guid " << guid << ", instance " << instanceId;
    luaL_error(L, "Unknown event type (%s)", oss.str().c_str());
    return 0;
}

/*
 * Cleans up the stack, effectively undoing all Push calls and the Setup call.
 */
void Eluna::CleanUpStack(int number_of_arguments)
{
    // Stack: event_id, [arguments]

    lua_pop(L, number_of_arguments + 1); // Add 1 because the caller doesn't know about `event_id`.
    // Stack: (empty)

    if (event_level == 0)
        InvalidateObjects();
}

/*
 * Call a single event handler that was put on the stack with `Setup` and removes it from the stack.
 *
 * The caller is responsible for keeping track of how many times this should be called.
 */
int Eluna::CallOneFunction(int number_of_functions, int number_of_arguments, int number_of_results)
{
    ++number_of_arguments; // Caller doesn't know about `event_id`.
    ASSERT(number_of_functions > 0 && number_of_arguments > 0 && number_of_results >= 0);
    // Stack: event_id, [arguments], [functions]

    int functions_top        = lua_gettop(L);
    int first_function_index = functions_top - number_of_functions + 1;
    int arguments_top        = first_function_index - 1;
    int first_argument_index = arguments_top - number_of_arguments + 1;

    // Copy the arguments from the bottom of the stack to the top.
    for (int argument_index = first_argument_index; argument_index <= arguments_top; ++argument_index)
    {
        lua_pushvalue(L, argument_index);
    }
    // Stack: event_id, [arguments], [functions], event_id, [arguments]

    ExecuteCall(number_of_arguments, number_of_results);
    --functions_top;
    // Stack: event_id, [arguments], [functions - 1], [results]

    return functions_top + 1; // Return the location of the first result (if any exist).
}

CreatureAI* Eluna::GetAI(Creature* creature)
{
    if (!IsEnabled())
        return nullptr;

    for (int i = 1; i < Hooks::CREATURE_EVENT_COUNT; ++i)
    {
        Hooks::CreatureEvents event_id = (Hooks::CreatureEvents)i;

        auto entryKey = EntryKey<Hooks::CreatureEvents>(event_id, creature->GetEntry());
        auto uniqueKey = UniqueObjectKey<Hooks::CreatureEvents>(event_id, creature->GET_GUID(), creature->GetInstanceId());

        if (CreatureEventBindings->HasBindingsFor(entryKey) ||
            CreatureUniqueBindings->HasBindingsFor(uniqueKey))
            return new ElunaCreatureAI(creature);
    }

    return nullptr;
}

GameObjectAI* Eluna::GetAI(GameObject* gameobject)
{
    if (!IsEnabled())
        return nullptr;

    for (int i = 1; i < Hooks::GAMEOBJECT_EVENT_COUNT; ++i)
    {
        Hooks::GameObjectEvents event_id = (Hooks::GameObjectEvents)i;

        auto entryKey = EntryKey<Hooks::GameObjectEvents>(event_id, gameobject->GetEntry());
        auto uniqueKey = UniqueObjectKey<Hooks::GameObjectEvents>(event_id, gameobject->GET_GUID(), gameobject->GetInstanceId());

        if (GameObjectEventBindings->HasBindingsFor(entryKey))
            return new ElunaGameObjectAI(gameobject);
    }

    return nullptr;
}

InstanceData* Eluna::GetInstanceData(Map* map)
{
    if (!IsEnabled())
        return NULL;

    for (int i = 1; i < Hooks::INSTANCE_EVENT_COUNT; ++i)
    {
        Hooks::InstanceEvents event_id = (Hooks::InstanceEvents)i;

        auto key = EntryKey<Hooks::InstanceEvents>(event_id, map->GetId());

        if (MapEventBindings->HasBindingsFor(key) ||
            InstanceEventBindings->HasBindingsFor(key))
            return new ElunaInstanceAI(map);
    }

    return NULL;
}

bool Eluna::HasInstanceData(Map const* map)
{
    if (!map->Instanceable())
        return continentDataRefs.find(map->GetId()) != continentDataRefs.end();
    else
        return instanceDataRefs.find(map->GetInstanceId()) != instanceDataRefs.end();
}

void Eluna::CreateInstanceData(Map const* map)
{
    ASSERT(lua_istable(L, -1));
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    if (ref == LUA_REFNIL)
        return;

    if (!map->Instanceable())
    {
        uint32 mapId = map->GetId();

        // If there's another table that was already stored for the map, unref it.
        auto mapRef = continentDataRefs.find(mapId);
        if (mapRef != continentDataRefs.end())
        {
            luaL_unref(L, LUA_REGISTRYINDEX, mapRef->second);
        }

        continentDataRefs[mapId] = ref;
    }
    else
    {
        uint32 instanceId = map->GetInstanceId();

        // If there's another table that was already stored for the instance, unref it.
        auto instRef = instanceDataRefs.find(instanceId);
        if (instRef != instanceDataRefs.end())
        {
            luaL_unref(L, LUA_REGISTRYINDEX, instRef->second);
        }

        instanceDataRefs[instanceId] = ref;
    }
}

/*
 * Unrefs the instanceId related events and data
 * Does all required actions for when an instance is freed.
 */
void Eluna::FreeInstanceId(uint32 instanceId)
{
    for (int i = 1; i < Hooks::INSTANCE_EVENT_COUNT; ++i)
    {
        auto key = EntryKey<Hooks::InstanceEvents>((Hooks::InstanceEvents)i, instanceId);

        if (MapEventBindings->HasBindingsFor(key))
            MapEventBindings->Clear(key);

        if (InstanceEventBindings->HasBindingsFor(key))
            InstanceEventBindings->Clear(key);

        if (instanceDataRefs.find(instanceId) != instanceDataRefs.end())
        {
            luaL_unref(L, LUA_REGISTRYINDEX, instanceDataRefs[instanceId]);
            instanceDataRefs.erase(instanceId);
        }
    }
}

void Eluna::PushInstanceData(lua_State* L, ElunaInstanceAI* ai, bool incrementCounter)
{
    // Check if the instance data is missing (i.e. someone reloaded Eluna).
    if (!HasInstanceData(ai->instance))
        ai->Reload();

    // Get the instance data table from the registry.
    if (!ai->instance->Instanceable())
        lua_rawgeti(L, LUA_REGISTRYINDEX, continentDataRefs[ai->instance->GetId()]);
    else
        lua_rawgeti(L, LUA_REGISTRYINDEX, instanceDataRefs[ai->instance->GetInstanceId()]);

    ASSERT(lua_istable(L, -1));

    if (incrementCounter)
        ++push_counter;
}

void Eluna::EnableSandboxForCall(lua_State* state)
{
    lua_getglobal(state, ELUNA_SAFE_MODE_ENV);
    lua_setupvalue(state, -2, 1); // First upvalue of file main chunk is environment
}

bool Eluna::IsSandboxed() const
{
    if (!L->ci->previous) // No caller / Full stack empty
        return true;
    // HACK to access LUA caller function environment
    // index2addr is working this way:
    //      L->ci->func + idx;
    // So let's trick LUA into thinking we are the previous function ... hum hum ...
    StkId saved = L->ci->func;
    L->ci->func = L->ci->previous->func - 1;
    lua_pushvalue(L, 1);
    L->ci->func = saved; // "You have seen nothing"
    // Stack: caller function

    // Let's get the '_ENV' variable. It is an upvalue of the LUA function.
    int n = 0;
    bool _env_found = false;
    while (const char* s = lua_getupvalue (L, -1, ++n))
    {
        if (strcmp(s, "_ENV") == 0)
        {
            _env_found = true;
            break;
        }
        lua_pop(L, 1);
    }
    if (!_env_found) // Should not happen .. ?
        return true;
    // Stack: _ENV of caller

    // Do we have access to global variable ?
    lua_pushstring(L, "_G");
    lua_rawget(L, -2);
    // Stack: _ENV of caller, _ENV[_G]
    lua_remove(L, -2);
    // Stack: _ENV[_G]
    if (lua_isnoneornil(L, -1))
    {
        lua_pop(L, 1);
        return true;
    }
    lua_pushglobaltable(L);
    // Stack: _ENV[_G], _G
    if (lua_rawequal(L, -1, -2))
    {
        lua_pop(L, 2);
        return false;
    }
    lua_pop(L, 2);
    return true;
}

LuaFileScriptLoader::LuaFileScriptLoader(const char* scriptname, const char* filename):
    LuaScriptLoader(scriptname)
{
    _file = fopen(filename, "r");
}

const char* LuaFileScriptLoader::Read(size_t& size)
{
    if (!_file)
        return nullptr;
    if (feof(_file))
        return nullptr;
    size = fread(_rbuf, 1, sizeof(_rbuf), _file);  /* read block */
    return _rbuf;
}

LuaStringScriptLoader::LuaStringScriptLoader(const char* scriptname, std::string const& content):
    LuaScriptLoader(scriptname), _read(false), _content(content)
{
}

const char* LuaStringScriptLoader::Read(size_t& size)
{
    if (_read)
        return nullptr;
    _read = true;
    size = _content.size();
    return _content.c_str();
}
