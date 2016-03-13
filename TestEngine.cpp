#include "TestEngine.h"

TestEngine::TestEngine(): Eluna(nullptr), L2(nullptr)
{
}

TestEngine::~TestEngine()
{
}

void TestEngine::StartTests()
{
    if (IsRunning())
        return;

    const char* test_framework_file = "test_framework";
    // Load testing framework first
    LuaFileScriptLoader loader(test_framework_file, (lua_folderpath + "/extensions/" + test_framework_file + ".lua").c_str());
    if (!loader.ok())
    {
        sLog.outError("Tests: Unable to open file %s", test_framework_file);
        return;
    }
    bool run_framework_result = RunScript(loader);
    lua_pop(L, 1);
    if (!run_framework_result)
    {
        sLog.outError("Tests: Unable to load framework file %s", test_framework_file);
        return;
    }
    for (auto itr = Eluna::luaTestsSQL.begin(); itr != Eluna::luaTestsSQL.end(); ++itr)
        RunSQLScript(itr->first, itr->second);

    const char* run_tests_function = "RunAllTests";

    // Now we call "RunAllTests" function in a coroutine
    L2 = lua_newthread(L);
    lua_getglobal(L2, run_tests_function);
    if (!lua_isfunction(L2, 1))
    {
        sLog.outError("Tests: Function %s is not defined. Unable to start tests.", run_tests_function);
        lua_close(L2);
        L2 = nullptr;
    }
}

void TestEngine::Update()
{
    if (!L2)
        return;

    int result = lua_resume(L2, NULL, 0);
    if (result == LUA_YIELD) // Yield, we will continue at next update tick
        return;

    if (!result) // Finished OK
        sLog.outString("Tests finished");
    else
        sLog.outError("Tests interrupted by errors");

    // L2 is already freed up. No need to call lua_close.
    L2 = nullptr;
}
