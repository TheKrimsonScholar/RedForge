#include <iostream>

#include "Engine.h"

int main(int argc, char* argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    if(!LoadGameLibrary(std::filesystem::path(GAME_PATH)))
        return EXIT_FAILURE;

    for(TypeID componentType : TypeRegistry::GetRegisteredComponentsList())
        std::cout << GET_TYPE_NAME(componentType) << std::endl;

    Engine* engine = new Engine();

    engine->Run();

    delete engine;
}