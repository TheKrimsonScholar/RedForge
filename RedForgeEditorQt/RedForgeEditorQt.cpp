#include <QtWidgets/QApplication>

#include "MainEditorWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MainEditorWindow window;
    window.show();
    return app.exec();
}