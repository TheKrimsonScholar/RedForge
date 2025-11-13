#pragma once

#include <QtWidgets/QMainWindow>

class MainEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainEditorWindow(QWidget* parent = nullptr);
    ~MainEditorWindow();
};