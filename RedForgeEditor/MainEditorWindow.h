#pragma once

#include "VulkanViewport.h"

#include "Exports.h"
#define VK_USE_PLATFORM_WIN32_KHR

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>

#include "Engine.h"

#include "ViewportCamera.h"

class MainEditorWindow : public Gtk::Window
{
protected:
    Engine* engine;

    Gtk::HeaderBar* headerBar;
    Gtk::Paned* mainPanel;
    Gtk::Paned* leftPanel;
    Gtk::Paned* rightPanel;

    VulkanViewport viewport;

public:
    MainEditorWindow();
    ~MainEditorWindow() override;
};