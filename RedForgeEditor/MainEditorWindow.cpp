#include "MainEditorWindow.h"

#include <iostream>

#include <gtkmm/drawingarea.h>
#include <glibmm/main.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <vulkan/vulkan.h>
#include "Engine.h"

#include <gdk/gdk.h>
#include <gdk/win32/gdkwin32.h>
#include <glibmm.h>
#include <gdkmm/pixbuf.h>

MainEditorWindow::MainEditorWindow() : 
    viewport()
{
    engine = new Engine();
    viewport.engine = engine;

    //engine->Startup();

    set_title("RedForge Editor");
    set_default_size(1280, 720);

    headerBar = new Gtk::HeaderBar();
    set_titlebar(*headerBar);

    {
        // Main horizontal split
        mainPanel = new Gtk::Paned(Gtk::Orientation::HORIZONTAL);

        // Left dock area
        leftPanel = new Gtk::Paned(Gtk::Orientation::VERTICAL);
        auto left_top = new Gtk::Frame("Properties");
        auto left_bottom = new Gtk::Frame("Hierarchy");
        leftPanel->set_start_child(*left_top);
        leftPanel->set_end_child(*left_bottom);

        // Right side - viewport and bottom dock
        rightPanel = new Gtk::Paned(Gtk::Orientation::VERTICAL);
        rightPanel->set_start_child(viewport);
        auto bottom_dock = new Gtk::Frame("Console");
        rightPanel->set_end_child(*bottom_dock);

        // Combine
        mainPanel->set_start_child(*leftPanel);
        mainPanel->set_end_child(*rightPanel);

        // Set initial positions
        leftPanel->set_position(200);
        mainPanel->set_position(250);
        rightPanel->set_position(400);

        set_child(*mainPanel);
    }

    viewport.onViewportInitialized = [this]()
        {
            engine->Startup(true, viewport.get_allocated_width(), viewport.get_allocated_height());

            CameraManager::SetViewMatrixOverride(&viewport.camera->viewMatrix);
            CameraManager::SetProjectionMatrixOverride(&viewport.camera->projectionMatrix);
        };
    viewport.onViewportUpdated = [this]()
        {
            engine->Update();
        };

    present();
}
MainEditorWindow::~MainEditorWindow()
{
    engine->Shutdown();

    delete rightPanel;
    delete leftPanel;
    delete mainPanel;
    delete headerBar;

    delete engine;
}