#pragma once

#include <QtWidgets/QMainWindow>

#include "VulkanViewport.h"
#include "EditorToolbar.h"
#include "HierarchyPanel.h"
#include "FileBrowserPanel.h"
#include "InspectorPanel.h"
#include "ConsolePanel.h"

class Engine;

class MainEditorWindow : public QMainWindow
{
    Q_OBJECT

private:
    static inline MainEditorWindow* Instance;

    Engine* engine;
    EditorToolbar* toolbar;
    VulkanViewport* viewport;
    HierarchyPanel* hierarchyPanel;
    FileBrowserPanel* fileBrowserPanel;
    InspectorPanel* inspectorPanel;
    ConsolePanel* consolePanel;

    QtInputLayer* inputLayer;

public:
    MainEditorWindow(QWidget* parent = nullptr);
    ~MainEditorWindow();

protected:
    void Initialize();
    void Update();

    void changeEvent(QEvent* event) override;

    void closeEvent(QCloseEvent* event) override;

    void OnWindowMinimized();
    void OnWindowRestored();
    void OnWindowMaximized();

public:
    static MainEditorWindow* Get() { return Instance; }

    HierarchyPanel* GetHierarchyPanel() const { return hierarchyPanel; }
    FileBrowserPanel* GetFileBrowserPanel() const { return fileBrowserPanel; }
    InspectorPanel* GetInspectorPanel() const { return inspectorPanel; }
};