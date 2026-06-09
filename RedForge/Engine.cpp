#include "Engine.h"

#include <iostream>

#include "LevelManager.h"

#include "GraphicsSystem.h"

#include "GraphicsState.h"

#include "TransformComponent.h"
#include "DefaultCameraControllerComponent.h"
#include "GLFWInputLayer.h"

#include "DebugMacros.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#ifdef _WIN32
    #undef GetCurrentTime()
#endif

void Engine::Run()
{
    if(isRunning)
		return;

    Startup();

    // Load default level
    world.GetEntityManager().LoadLevel(L"Levels/Level.txt");

    // TEMP: Main camera creation
    {
        TransformComponent transform{};
        transform.location = { 0, 3, 5 };
        transform.rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        transform.scale = { 1, 1, 1 };

        CameraComponent camera{};
        camera.projectionType = ECameraProjectionType::Perspective;
		camera.fov = glm::radians(45.0f);
		camera.aspectRatio = world.GetResource<const Window>().GetAspectRatio();
		camera.nearClipPlaneDistance = 0.1f;
		camera.farClipPlaneDistance = 100.0f;

        DefaultCameraControllerComponent cameraController {};

	    Entity cameraEntity = world.GetEntityManager().CreateEntity();
	    world.GetEntityManager().AddComponent<TransformComponent>(cameraEntity, transform);
	    world.GetEntityManager().AddComponent<CameraComponent>(cameraEntity, camera);
		world.GetEntityManager().AddComponent<DefaultCameraControllerComponent>(cameraEntity, cameraController);
        world.GetResource<Cameras>().mainCameraEntity = cameraEntity;
    }

    double lastFrameTime = glfwGetTime();
    while(!glfwWindowShouldClose(world.GetResource<Window>().window))
    {
		glfwPollEvents();

        double deltaTime = glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();

        Update(deltaTime);
    }

    Shutdown();
}

void Engine::CreateVulkanInstance()
{
    static_cast<GraphicsSystem*>(scheduler.systems[typeid(GraphicsSystem)])->CreateVulkanInstance_PreStartup(world.GetResource<GraphicsState>());
}
void Engine::DestroyVulkanInstance()
{
    static_cast<GraphicsSystem*>(scheduler.systems[typeid(GraphicsSystem)])->DestroyVulkanInstance_PostShutdown(world.GetResource<GraphicsState>());
}

void Engine::Startup(const EngineStartupParams& params)
{
    if(isRunning)
		return;
	isRunning = true;

	scheduler.Startup(params, world);
    
    //for(size_t i = 0; i < archetypes.size(); i++)
        //archetypes[i]->Startup(params, world);

    /*Entity player = Player::Get()->ConstructEntity({});
    Entity enemy = Enemy::Get()->ConstructEntity({});

    Player::QueuePlayerEvent1(player, {});
    Player::QueuePlayerEvent1(player, {});
    Enemy::QueuePlayerEvent1(enemy, {});*/
    LOG(world, "---- RedForge Version 0.1 ----");
}
void Engine::Shutdown(const EngineShutdownParams& params)
{
    if(!isRunning)
        return;

    // Wait for device to finish operations before exiting
    vkDeviceWaitIdle(world.GetResource<GraphicsState>().device);

	scheduler.Shutdown(params, world);

    for(size_t i = 0; i < archetypes.size(); i++)
        delete archetypes[i];
    archetypes.clear();

    isRunning = false;
}

void Engine::Update(float deltaTime)
{
    if(!isRunning)
        return;

    glfwPollEvents();

	scheduler.Update(deltaTime);

    //for(size_t i = 0; i < archetypes.size(); i++)
        //archetypes[i]->Update();
}

void Engine::QueueExternalTask(std::function<void(World&)> callback)
{
	scheduler.QueueExternalTask(callback);
}