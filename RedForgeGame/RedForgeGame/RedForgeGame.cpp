#include <iostream>

#include "Engine.h"

int main(int argc, char* argv[])
{
    if(!LoadGameLibrary(std::filesystem::path(GAME_PATH)))
        return EXIT_FAILURE;

    for(std::type_index componentType : GetRegisteredComponentsList())
        std::cout << componentType.name() << std::endl;

    Engine* engine = new Engine();

    engine->Run();

    delete engine;
}