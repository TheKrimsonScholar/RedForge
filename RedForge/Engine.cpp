#include "Engine.h"

#include <iostream>

#include "LevelManager.h"

#include "GraphicsSystem.h"

#include "GraphicsState.h"

#include "TransformComponent.h"
#include "InputComponent.h"
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

  //  // TEMP: Main camera creation
  //  {
  //      TransformComponent transform{};
  //      transform.location = { 0, 3, 5 };
  //      transform.rotation = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  //      transform.scale = { 1, 1, 1 };

  //      CameraComponent camera{};
  //      camera.projectionType = ECameraProjectionType::Perspective;
		//camera.fov = glm::radians(45.0f);
		//camera.aspectRatio = GraphicsSystem::GetAspectRatio();
		//camera.nearClipPlaneDistance = 0.1f;
		//camera.farClipPlaneDistance = 100.0f;

  //      InputComponent input{};
  //      float lookSpeed = 10.0f;
  //      input.mouseDownCallbacks.emplace((uint32_t) MouseButtonCode::Left, [lookSpeed](Entity e)
  //          {
		//		static const float MAX_PITCH_ANGLE = 85.0f;

		//		glm::dvec2 mouseDelta = InputSystem::GetMouseDelta();
  //              if(mouseDelta == glm::zero<glm::dvec2>())
  //                  return;

		//		TransformComponent& transform = EntityManager::GetComponent<TransformComponent>(e);

  //              float yawAmount = mouseDelta.x * lookSpeed * TimeManager::GetDeltaTime();
  //              float pitchAmount = mouseDelta.y * lookSpeed * TimeManager::GetDeltaTime();

  //              // Rotation around world Y-axis
  //              glm::quat yawRotation = glm::angleAxis(-yawAmount, glm::vec3(0.0f, 1.0f, 0.0f));
  //              transform.rotation = yawRotation * transform.rotation;

  //              // Rotation around camera's right axis
  //              glm::quat pitchRotation = glm::angleAxis(-pitchAmount, transform.GetRight());

  //              // Forward after applying pitch
  //              glm::vec3 newForward = (pitchRotation * transform.rotation) * glm::vec3(0.0f, 0.0f, -1.0f);
  //              float newPitchAngle = glm::degrees(glm::asin(newForward.y));

  //              // Apply pitch only if within bounds
  //              if(newPitchAngle < MAX_PITCH_ANGLE && newPitchAngle > -MAX_PITCH_ANGLE)
  //                  transform.rotation = pitchRotation * transform.rotation;

  //              // Enforce roll lock by using world up as the up vector for lookAt
  //              transform.rotation = glm::quatLookAt(transform.GetForward(), glm::vec3(0.0f, 1.0f, 0.0f));
  //              transform.rotation = glm::normalize(transform.rotation);
  //          });
		//float movementSpeed = 5.0f;
  //      input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::D, [movementSpeed](Entity e)
  //          {
  //              EntityManager::GetComponent<TransformComponent>(e).location += EntityManager::GetComponent<TransformComponent>(e).GetRight() * movementSpeed * TimeManager::GetDeltaTime();
  //          });
  //      input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::A, [movementSpeed](Entity e)
  //          {
  //              EntityManager::GetComponent<TransformComponent>(e).location -= EntityManager::GetComponent<TransformComponent>(e).GetRight() * movementSpeed * TimeManager::GetDeltaTime();
  //          });
  //      input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::SPACE, [movementSpeed](Entity e)
  //          {
  //              EntityManager::GetComponent<TransformComponent>(e).location += glm::vec3(0, 1, 0) * movementSpeed * TimeManager::GetDeltaTime();
  //          });
  //      input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::LSHIFT, [movementSpeed](Entity e)
  //          {
  //              EntityManager::GetComponent<TransformComponent>(e).location -= glm::vec3(0, 1, 0) * movementSpeed * TimeManager::GetDeltaTime();
  //          });
  //      input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::W, [movementSpeed](Entity e)
  //          {
  //              EntityManager::GetComponent<TransformComponent>(e).location += EntityManager::GetComponent<TransformComponent>(e).GetForward() * movementSpeed * TimeManager::GetDeltaTime();
  //          });
  //      input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::S, [movementSpeed](Entity e)
  //          {
  //              EntityManager::GetComponent<TransformComponent>(e).location -= EntityManager::GetComponent<TransformComponent>(e).GetForward() * movementSpeed * TimeManager::GetDeltaTime();
  //          });

	 //   Entity cameraEntity = EntityManager::CreateEntity();
	 //   EntityManager::AddComponent<TransformComponent>(cameraEntity, transform);
	 //   EntityManager::AddComponent<CameraComponent>(cameraEntity, camera);
		//EntityManager::AddComponent<InputComponent>(cameraEntity, input);
  //      CameraManager::SetMainCamera(cameraEntity);
  //  }

    while(!glfwWindowShouldClose(world.GetResource<Window>().window))
    {
		glfwPollEvents();

	    /*timeManager.Update();
        inputSystem.Update();
        physicsSystem.Update();
        networkSystem.Update();
        graphics.Update();*/
    }

    Shutdown();
}

void Engine::CreateVulkanInstance()
{
    static_cast<GraphicsSystem*>(GET_SYSTEM_INSTANCE(typeid(GraphicsSystem)))->CreateVulkanInstance_PreStartup(world.GetResource<GraphicsState>());
}
void Engine::DestroyVulkanInstance()
{
    static_cast<GraphicsSystem*>(GET_SYSTEM_INSTANCE(typeid(GraphicsSystem)))->DestroyVulkanInstance_PostShutdown(world.GetResource<GraphicsState>());
}

void Engine::Startup(const EngineStartupParams& params)
{
    if(isRunning)
		return;
	isRunning = true;

    /*timeManager.Startup(params, world);
    windowSystem.Startup(params, world);
    resourceManager.Startup(params, world);
    inputSystem.Startup(params, world);
    entityManager.Startup();
    graphics.Startup(params, world);
    cameraManager.Startup(params, world);
    debugManager.Startup(params, world);
    physicsSystem.Startup(params, world);
	fileManager.Startup();
	levelManager.Startup();
	networkSystem.Startup();*/

    //archetypes.push_back(new Player());
    //archetypes.push_back(new Enemy());

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

    /*networkSystem.Shutdown();
	levelManager.Shutdown();
	fileManager.Shutdown();
    physicsSystem.Shutdown(params, world);
    debugManager.Shutdown(params, world);
    cameraManager.Shutdown(params, world);
    entityManager.Shutdown();
    inputSystem.Shutdown(params, world);
    resourceManager.Shutdown(params, world);
    graphics.Shutdown(params, world);
    windowSystem.Shutdown(params, world);
    timeManager.Shutdown(params, world);*/

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

    /*timeManager.Update();
    inputSystem.Update();
    physicsSystem.Update();
    networkSystem.Update();
    graphics.Update();
    debugManager.Update();*/

    //for(size_t i = 0; i < archetypes.size(); i++)
        //archetypes[i]->Update();

    //EntityManager::GetComponent<TransformComponent>(0).rotation = glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(1, 0, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 1, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 0, 1)) * EntityManager::GetComponent<TransformComponent>(0).rotation;

    /*DebugManager::DrawDebugBox(
        glm::vec3(2, 1, 1), glm::angleAxis(glm::radians(60.0f * (float) world.GetResource<Time>().GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugManager::DrawDebugSphere(
        glm::vec3(2, 3, 1), glm::angleAxis(glm::radians(60.0f * (float)world.GetResource<Time>().GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));*/
}

void Engine::QueueExternalTask(std::function<void(World&)> callback)
{
	scheduler.QueueExternalTask(callback);
}