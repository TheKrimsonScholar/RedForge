#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <chrono>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#include "Engine.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Engine* engine = new Engine();
    engine->Run();

    delete engine;

    return 0;
}