#include "TestEngine.h"

TestEngine::TestEngine(): Eluna(nullptr), L2(nullptr)
{
    StartTests();
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
    LuaFileScriptLoader loader(test_framework_file, (lua_folderpath + "/" + test_framework_file + ".lua").c_str());
    bool run_framework_result = RunScript(loader);
    lua_pop(L, 1);
    if (!run_framework_result)
    {
        sLog.outError("Tests: Unable to load framework file %s", test_framework_file);
        return;
    }
    for (auto itr = Eluna::luaTestsSQL.begin(); itr != Eluna::luaTestsSQL.end(); ++itr)
        RunSQLScript(itr->first, itr->second);

    // Now we call "RunAllTests" function in a coroutine
    L2 = lua_newthread(L);
    lua_getglobal(L2, "RunAllTests");
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
    L2 = nullptr;
}
