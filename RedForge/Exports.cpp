#include "Exports.h"

#include "Engine.h"

int Get()
{
    return 4;
}

void Run()
{
    #ifdef _WIN32
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif
    
    Engine* engine = new Engine();
    engine->Run();
    
    delete engine;
}