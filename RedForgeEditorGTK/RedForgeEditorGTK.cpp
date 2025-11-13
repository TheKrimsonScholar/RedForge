#include <QtWidgets/QApplication>

#include "MainEditorWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MainEditorWindow window;
    window.show();
    return app.exec();
}

////#define _CRTDBG_MAP_ALLOC
////#include <crtdbg.h>
//#include <chrono>

//#include <iostream>

//#include "MainEditorWindow.h"

//int main(int argc, char* argv[])
//{
//    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//
//    std::cout << GetEnginePath() << std::endl;
//    std::cout << GetEngineAssetsPath() << std::endl;
//
//    // Check if the DLL path was provided as an argument
//    if(argc < 2)
//    {
//        std::cerr << "Usage: " << argv[0] << " <path_to_dll>" << std::endl;
//        return EXIT_FAILURE;
//    }
//
//    // The first argument (argv[1]) is the game path. The game DLL is contained here.
//    std::string gameDLLPath = argv[1];
//    
//    if(!LoadGameLibrary(gameDLLPath))
//        return EXIT_FAILURE;
//
//    for(std::type_index componentType : GetRegisteredComponentsList())
//        std::cout << componentType.name() << std::endl;
//
//    // Disable hardware acceleration (causes flickering on Windows)
//    g_setenv("GSK_RENDERER", "cairo", TRUE);
//    auto app = Gtk::Application::create("org.krimson.RedForgeEditor", Gio::Application::Flags::NON_UNIQUE); // Allow multiple instances of the app for testing
//
//    /* Apply CSS styles */
//    try
//    {
//        auto provider = Gtk::CssProvider::create();
//        provider->load_from_path(GetEditorAssetsPath().append(L"styles/style.css").string());
//
//        Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
//    }
//    catch(const Glib::Error& ex)
//    {
//        std::cerr << "Error loading CSS: " << ex.what() << std::endl;
//        return 1;
//    }
//
//    int result = app->make_window_and_run<MainEditorWindow>(1, argv); // Because we take the path to the game DLL as a commandline argument, only pass the first argument (EXE path) to GTK
//    return result;
//}