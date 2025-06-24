//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
#include <chrono>

#include <iostream>

#include "MainEditorWindow.h"

int main(int argc, char* argv[])
{
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Disable hardware acceleration (causes flickering on Windows)
    g_setenv("GSK_RENDERER", "cairo", TRUE);
    auto app = Gtk::Application::create("org.krimson.RedForgeEditor");

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

    int result = app->make_window_and_run<MainEditorWindow>(argc, argv);
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