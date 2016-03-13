#ifndef TESTENGINE_H
#define TESTENGINE_H

#include "LuaEngine.h"

class TestEngine: protected Eluna
{
public:
    TestEngine();
    ~TestEngine();

    void StartTests();
    void Update();
    bool IsRunning() const { return L2 != nullptr; }

    static TestEngine* instance()
    {
        static TestEngine t;
        return &t;
    }
protected:
    lua_State *L2; // Test thread
};

#define sTestEngine (TestEngine::instance())
#endif // TESTENGINE_H
