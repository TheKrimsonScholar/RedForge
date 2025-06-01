#include <iostream>

#include "Exports.h"
#include "Engine.h"

#include "ViewportCamera.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <gtkmm.h>
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>

class MainEditorWindow : public Gtk::Window
{
protected:
    Gtk::Button m_button;
    Gtk::DrawingArea m_drawingArea;

public:
    MainEditorWindow() : m_button("Click Me!"), m_drawingArea()
    {
        set_title("My Gtkmm App");
        set_default_size(1280, 720);

        //Glib::ConstructParams k(,);

        // Set the button as the child of the window
        set_child(m_button);

        // Connect the button's clicked signal to a member function
        m_button.signal_clicked().connect(sigc::mem_fun(*this, &MainEditorWindow::on_button_clicked));
    }
    ~MainEditorWindow() override {}

protected:
    // Signal handler for the button click
    void on_button_clicked()
    {
        std::cout << "Hello, Gtkmm World!" << std::endl;

        m_drawingArea.queue_draw();
    }
};

int main(int argc, char* argv[]) {
    // Create a Gtk::Application object
    auto app = Gtk::Application::create("org.gtkmm.example");

    // Run the application with your custom window
    return app->make_window_and_run<MainEditorWindow>(argc, argv);
}

int main1()
{
    std::cout << "Hello World!\n";

    std::cout << Get() << std::endl;

    return 0;

    Engine* engine = new Engine();

    /*{
        engine->Run();

        delete engine;
        return 0;
    }*/

    engine->Startup();

    TimeManager& timeManager = engine->GetTimeManager();
    InputSystem& inputSystem = engine->GetInputSystem();
    GraphicsSystem& graphics = engine->GetGraphicsSystem();
    PhysicsSystem& physicsSystem = engine->GetPhysicsSystem();

    /*glm::mat4* viewMatrix = new glm::mat4(1.0f);
    glm::mat4* projectionMatrix = new glm::mat4(1.0f);*/

    ViewportCamera* camera = new ViewportCamera();

    CameraManager::SetViewMatrixOverride(&camera->viewMatrix);
    CameraManager::SetProjectionMatrixOverride(&camera->projectionMatrix);

    /*{
        TransformComponent transform{};
        transform.location = { 0, 3, 5 };
        transform.rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        transform.scale = { 1, 1, 1 };

        CameraComponent camera{};
        camera.projectionType = ECameraProjectionType::Perspective;
        camera.fov = glm::radians(45.0f);
        camera.aspectRatio = GraphicsSystem::GetAspectRatio();
        camera.nearClipPlaneDistance = 0.1f;
        camera.farClipPlaneDistance = 100.0f;

        Entity cameraEntity = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(cameraEntity, transform);
        EntityManager::AddComponent<CameraComponent>(cameraEntity, camera);
        CameraManager::SetMainCamera(cameraEntity);
    }*/

    {
        TransformComponent transform{};
        transform.location = { 1, 7, 0 };
        transform.rotation = glm::angleAxis(45.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        transform.scale = { 0.25f, 0.25f, 0.25f };

        MeshRendererComponent renderer{};
        renderer.UseMeshDefaults(L"KhaimBook.obj");

        ColliderComponent collider{};
        collider.colliderType = EColliderType::Box;
        collider.halfSize = glm::vec3(0.4f, 0.2f, 0.4f);
        collider.center = glm::vec3(0.0f, 0.0f, 0.0f);

        PhysicsComponent physics{};
        physics.gravity = glm::vec3(0, -1.81f, 0);
        physics.mass = 1;
        physics.isStatic = false;

        Entity entity = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(entity, transform);
        EntityManager::AddComponent<MeshRendererComponent>(entity, renderer);
        EntityManager::AddComponent<ColliderComponent>(entity, collider);
        EntityManager::AddComponent<PhysicsComponent>(entity, physics);
    }

    GLFWwindow* window = GraphicsSystem::GetWindow();
    while(!glfwWindowShouldClose(window))
    {
		glfwPollEvents();

        camera->Update(window);

	    timeManager.Update();
        inputSystem.Update();
        graphics.Update();
        //physicsSystem.Update();

		//EntityManager::GetComponent<TransformComponent>(0).rotation = glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(1, 0, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 1, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 0, 1)) * EntityManager::GetComponent<TransformComponent>(0).rotation;

    //    DebugManager::DrawDebugBox(
    //        glm::vec3(2, 1, 1), glm::angleAxis(glm::radians(60.0f * (float) TimeManager::GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    //    DebugManager::DrawDebugSphere(
    //        glm::vec3(2, 3, 1), glm::angleAxis(glm::radians(60.0f * (float) TimeManager::GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    //    for(Entity e = 0; e < EntityManager::GetLastEntity(); e++)
    //        if(EntityManager::HasComponent<TransformComponent>(e))
    //        {
				////EntityManager::GetComponent<TransformComponent>(e).location += glm::vec3(0.0f, 0.0f, 1.0f) * TimeManager::GetDeltaTime();
    //            //EntityManager::GetComponent<TransformComponent>(e).rotation *= glm::angleAxis(glm::radians(60.0f * TimeManager::GetDeltaTime()), glm::normalize(glm::vec3(0, 0, 1)));
    //            //EntityManager::GetComponent<TransformComponent>(e).rotation = glm::angleAxis(glm::radians(60.0f * TimeManager::GetDeltaTime()), glm::normalize(glm::vec3(0, 0, 1))) * EntityManager::GetComponent<TransformComponent>(e).rotation;
				////EntityManager::GetComponent<TransformComponent>(e).scale += glm::vec3(-0.01f, -0.01f, -0.01f) * TimeManager::GetDeltaTime();
    //        }
    }

    delete camera;

    engine->Shutdown();

    delete engine;
}