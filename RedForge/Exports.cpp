#include "Exports.h"

#include "Engine.h"

int Get()
{
    return 4;
}

void Run()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    Engine* engine = new Engine();
    engine->Run();
    
    delete engine;
}