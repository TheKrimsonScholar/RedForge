//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
#include <chrono>

#include <iostream>

#include "MainEditorWindow.h"

int main(int argc, char* argv[])
{
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Check if the DLL path was provided as an argument
    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_dll>" << std::endl;
        return EXIT_FAILURE;
    }

    // The first argument (argv[1]) is the DLL path
    std::string gameDLLPath = argv[1];
    std::string gameName = gameDLLPath.substr(gameDLLPath.find_last_of("/\\") + 1);
    gameDLLPath += "/" + gameName;
    std::cout << "DLL Path: " << gameDLLPath << std::endl;

    #ifdef NETMODE_CLIENT
    gameDLLPath += "Client.dll";
    #elif NETMODE_SERVER
    gameDLLPath += "Server.dll";
    #else
    gameDLLPath += ".dll";
    #endif

    // TO-DO: Use argv[1] w/ client/server paths
    // Load the DLL using the provided path
    HMODULE handle = LoadLibraryA(gameDLLPath.c_str());
    if(handle == NULL)
    {
        std::cerr << "Failed to load DLL at path: " << gameDLLPath << std::endl;
        return EXIT_FAILURE;
    }

    for(std::type_index componentType : GetRegisteredComponentsList())
        std::cout << componentType.name() << std::endl;

    // Disable hardware acceleration (causes flickering on Windows)
    g_setenv("GSK_RENDERER", "cairo", TRUE);
    auto app = Gtk::Application::create("org.krimson.RedForgeEditor", Gio::Application::Flags::NON_UNIQUE); // Allow multiple instances of the app for testing

    /* Apply CSS styles */
    try
    {
        auto provider = Gtk::CssProvider::create();
        provider->load_from_path("styles/style.css");

        Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
    catch(const Glib::Error& ex)
    {
        std::cerr << "Error loading CSS: " << ex.what() << std::endl;
        return 1;
    }

    std::cout << "kjhjk" << std::endl;
    int result = app->make_window_and_run<MainEditorWindow>(1, argv); // Because we take the path to the game DLL as a commandline argument, only pass the first argument (EXE path) to GTK
    return result;
}

int main1()
{
    /*_CrtMemState initialMemoryState;
    _CrtMemCheckpoint(&initialMemoryState);*/

    {
        std::cout << "Hello World!\n";

        std::cout << Get() << std::endl;

        Engine* engine = new Engine();

        engine->Run();

        delete engine;
    }
    
    // Check difference in memory state to find leaks
    /*_CrtMemState finalMemoryState;
    _CrtMemCheckpoint(&finalMemoryState);
    _CrtMemState memoryDiff;
    if(_CrtMemDifference(&memoryDiff, &initialMemoryState, &finalMemoryState))
        _CrtMemDumpStatistics(&memoryDiff);*/
    
    return 0;
}