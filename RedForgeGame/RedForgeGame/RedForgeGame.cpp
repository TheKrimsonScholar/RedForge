#include <iostream>

#include "Engine.h"

int main(int argc, char* argv[])
{
    if(!LoadGameLibrary(std::filesystem::path(GAME_PATH)))
        return EXIT_FAILURE;

    for(TypeID componentType : TypeRegistry::GetRegisteredComponentsList())
        std::cout << GET_TYPE_NAME(componentType) << std::endl;

    Engine* engine = new Engine();

    engine->Run();

    delete engine;
}