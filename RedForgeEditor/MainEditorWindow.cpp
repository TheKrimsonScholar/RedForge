#include "MainEditorWindow.h"

#include <iostream>

#include <gtkmm/drawingarea.h>
#include <glibmm/main.h>

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <gdk/win32/gdkwin32.h>
#else
    #define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <vulkan/vulkan.h>
#include "Engine.h"

#include "DebugMacros.h"

#include <gdk/gdk.h>
#include <glibmm.h>
#include <gdkmm/pixbuf.h>

#include "ConsolePanel.h"

MainEditorWindow::MainEditorWindow() : Gtk::Window(),
    viewport()
{
    set_decorated(true);
    set_resizable(true);

    engine = new Engine();
    viewport.engine = engine;

    //engine->Startup();

    set_title("RedForge Editor");
    set_default_size(1280, 720);

    headerBar = Gtk::manage(new Gtk::HeaderBar());
    set_titlebar(*headerBar);

    {
        windowPanel = Gtk::manage(new Gtk::Paned(Gtk::Orientation::VERTICAL));

        toolbar = Gtk::manage(new EditorToolbar());

        // Main horizontal split
        mainPanel = Gtk::manage(new Gtk::Paned(Gtk::Orientation::HORIZONTAL));

        // Left dock area
        leftPanel = Gtk::manage(new Gtk::Paned(Gtk::Orientation::VERTICAL));
        inspectorWindow = Gtk::manage(new InspectorPanel());
        hierarchyPanel = Gtk::manage(new HierarchyPanel());
        hierarchyPanel->inspector = inspectorWindow;
        leftPanel->set_start_child(*inspectorWindow);
        leftPanel->set_end_child(*hierarchyPanel);

        // Right side - viewport and bottom dock
        rightPanel = Gtk::manage(new Gtk::Paned(Gtk::Orientation::VERTICAL));
        rightPanel->set_start_child(viewport);
        auto bottomDock = Gtk::manage(new ConsolePanel());
        rightPanel->set_end_child(*bottomDock);

        // Combine
        windowPanel->set_start_child(*toolbar);
        windowPanel->set_end_child(*mainPanel);

        mainPanel->set_start_child(*leftPanel);
        mainPanel->set_end_child(*rightPanel);

        // Set initial positions
        windowPanel->set_position(64);
        mainPanel->set_position(400);
        leftPanel->set_position(200);
        rightPanel->set_position(400);

        set_child(*windowPanel);
    }

    viewport.onViewportInitialized = [this]()
        {
            engine->Startup(true, viewport.get_allocated_width(), viewport.get_allocated_height());

            CameraManager::SetViewMatrixOverride(&viewport.camera->viewMatrix);
            CameraManager::SetProjectionMatrixOverride(&viewport.camera->projectionMatrix);

            {
                TransformComponent transform{};
                transform.location = { 1, 7, 0 };
                transform.rotation = glm::angleAxis(45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                transform.scale = { 0.25f, 0.25f, 0.25f };

                MeshRendererComponent renderer{};
                //renderer.UseMeshDefaults(L"KhaimBook.obj");

                ColliderComponent collider{};
                collider.colliderType = EColliderType::Box;
                collider.halfSize = glm::vec3(0.4f, 0.2f, 0.4f);
                collider.center = glm::vec3(0.0f, 0.0f, 0.0f);

                PhysicsComponent physics{};
                physics.gravity = glm::vec3(0, -1.81f, 0);
                physics.mass = 1;
                physics.isStatic = true;

                //Entity entity = LevelManager::CreateEntity();
                //EntityManager::AddComponent<TransformComponent>(entity, transform);
                /*EntityManager::AddComponent<MeshRendererComponent>(entity, renderer);
                EntityManager::AddComponent<ColliderComponent>(entity, collider);
                EntityManager::AddComponent<PhysicsComponent>(entity, physics);*/
            }

            LevelManager::LoadLevel();

            hierarchyPanel->UpdateHierarchy();
        };
	viewport.onViewportResized = [this]()
		{
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

    delete engine;
}