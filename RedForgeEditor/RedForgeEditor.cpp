//#include <crtdbg.h>
//
//#include "Engine.h"
//
//#include <QtWidgets/QApplication>
//#include <QtWidgets/QWidget>
//#include <QtWidgets/QHBoxLayout>
//#include <QtGui/QVulkanWindow>
//#include <QtWidgets/QWidget>
//
//#include "VulkanViewport.h"
//
//// ... QVulkanInstance and MyVulkanWindow definitions ...
//
//int main(int argc, char* argv[])
//{
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//
//    QApplication app(argc, argv);
//
//    Engine* engine = new Engine();
//    engine->Startup(true);
//
//    QVulkanInstance vulkanInstance;
//    // ... create vulkanInstance ...
//
//    VulkanViewport* vulkanViewport = new VulkanViewport();
//    vulkanViewport->setVulkanInstance(&vulkanInstance);
//
//    // Create the QWidget wrapper for the QWindow
//    QWidget* vulkanWidgetWrapper = QWidget::createWindowContainer(vulkanViewport);
//
//    // Use the wrapper widget in a QWidget main window/layout
//    QWidget mainWindow;
//    QHBoxLayout* layout = new QHBoxLayout(&mainWindow);
//    layout->addWidget(vulkanWidgetWrapper);
//
//    // Add other Qt Widgets here, e.g., layout->addWidget(new QPushButton("UI"));
//
//    mainWindow.show();
//    return app.exec();
//}

#include <QApplication>
#include <QFile>
#include <QStyleFactory>

#include "MainEditorWindow.h"

#include "EditorPaths.h"

#define DEFAULT_GAME_PATH GetEnginePath().append("RedForgeGame")

int main(int argc, char* argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    std::cout << GetEnginePath() << std::endl;
    std::cout << GetEngineAssetsPath() << std::endl;
    
    std::filesystem::path gameDLLPath = DEFAULT_GAME_PATH;
    // Check if the DLL path was provided as an argument (if no game path was specified, use the default path)
    if(argc >= 2)
        // The first argument (argv[1]) is the game path. The game DLL is contained here.
        gameDLLPath = argv[1];
        
    if(!LoadGameLibrary(gameDLLPath))
        return EXIT_FAILURE;

    QApplication app(argc, argv);
    app.setWindowIcon(QICON_FROM_PATH("icon.ico"));
    app.setApplicationName("RedForgeEditor");
    app.setApplicationDisplayName("RedForge Editor");
    app.setStyle("Fusion");

    QFile file(GetEditorAssetsPath().append("styles/style.qss"));
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        file.close();

        app.setStyleSheet(styleSheet);
    }

    MainEditorWindow window;
    window.show();

    int result = app.exec();

    return result;
}