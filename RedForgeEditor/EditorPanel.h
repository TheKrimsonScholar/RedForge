#pragma once

#include <QDockWidget>
#include <QScrollArea>

class EditorPanel : public QDockWidget
{
    Q_OBJECT

protected:
    QWidget* titlebar;
    QScrollArea* scrollArea;
    QWidget* contentArea;

public:
    EditorPanel(const QString& title, QWidget* parent = nullptr);
    ~EditorPanel();

protected:
    // Called after initializing the engine.
    virtual void Initialize() = 0;
    // Called after each engine update.
    virtual void Update() = 0;

    friend class MainEditorWindow;
};