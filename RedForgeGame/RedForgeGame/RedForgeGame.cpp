#include <iostream>

#include "Engine.h"

int main(int argc, char* argv[])
{
    #ifdef NETMODE_CLIENT
    HMODULE handle = LoadLibraryA("RedForgeGameDLLClient.dll");
    #elif NETMODE_SERVER
    HMODULE handle = LoadLibraryA("RedForgeGameDLLServer.dll");
    #else
    HMODULE handle = LoadLibraryA("RedForgeGameDLL.dll");
    #endif

    for(std::type_index componentType : GetRegisteredComponentsList())
        std::cout << componentType.name() << std::endl;

    Engine* engine = new Engine();

    engine->Run();

    delete engine;
}