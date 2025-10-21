#pragma once

#include "VulkanViewport.h"

#include "Exports.h"
#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#else
    #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>

#include "Engine.h"

#include "EditorToolbar.h"
#include "HierarchyPanel.h"
#include "ConsolePanel.h"
#include "FileBrowserPanel.h"

#include "ViewportCamera.h"

class MainEditorWindow : public Gtk::Window
{
protected:
    Engine* engine;

    Gtk::HeaderBar* headerBar;
    Gtk::Paned* windowPanel;
    EditorToolbar* toolbar;
    Gtk::Paned* mainPanel;
    Gtk::Paned* leftPanel;
    Gtk::Paned* rightPanel;

    InspectorPanel* inspectorWindow;
    HierarchyPanel* hierarchyPanel;
    ConsolePanel* consolePanel;
    FileBrowserPanel* fileBrowserPanel;

    VulkanViewport viewport;

public:
    MainEditorWindow();
    ~MainEditorWindow() override;
};