#include "Engine.h"

#include <iostream>

#include "LevelManager.h"

#include "TransformComponent.h"
#include "InputComponent.h"
#include "GLFWInputLayer.h"

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
    LevelManager::LoadLevel();

    // TEMP: Main camera creation
    {
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

        InputComponent input{};
        float lookSpeed = 10.0f;
        input.mouseDownCallbacks.emplace((uint32_t) MouseButtonCode::Left, [lookSpeed](Entity e)
            {
				static const float MAX_PITCH_ANGLE = 85.0f;

				glm::dvec2 mouseDelta = InputSystem::GetMouseDelta();
                if(mouseDelta == glm::zero<glm::dvec2>())
                    return;

				TransformComponent& transform = EntityManager::GetComponent<TransformComponent>(e);

                float yawAmount = mouseDelta.x * lookSpeed * TimeManager::GetDeltaTime();
                float pitchAmount = mouseDelta.y * lookSpeed * TimeManager::GetDeltaTime();

                // Rotation around world Y-axis
                glm::quat yawRotation = glm::angleAxis(-yawAmount, glm::vec3(0.0f, 1.0f, 0.0f));
                transform.rotation = yawRotation * transform.rotation;

                // Rotation around camera's right axis
                glm::quat pitchRotation = glm::angleAxis(-pitchAmount, transform.GetRight());

                // Forward after applying pitch
                glm::vec3 newForward = (pitchRotation * transform.rotation) * glm::vec3(0.0f, 0.0f, -1.0f);
                float newPitchAngle = glm::degrees(glm::asin(newForward.y));

                // Apply pitch only if within bounds
                if(newPitchAngle < MAX_PITCH_ANGLE && newPitchAngle > -MAX_PITCH_ANGLE)
                    transform.rotation = pitchRotation * transform.rotation;

                // Enforce roll lock by using world up as the up vector for lookAt
                transform.rotation = glm::quatLookAt(transform.GetForward(), glm::vec3(0.0f, 1.0f, 0.0f));
                transform.rotation = glm::normalize(transform.rotation);
            });
		float movementSpeed = 5.0f;
        input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::D, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location += EntityManager::GetComponent<TransformComponent>(e).GetRight() * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::A, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location -= EntityManager::GetComponent<TransformComponent>(e).GetRight() * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::SPACE, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location += glm::vec3(0, 1, 0) * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::LSHIFT, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location -= glm::vec3(0, 1, 0) * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::W, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location += EntityManager::GetComponent<TransformComponent>(e).GetForward() * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace((uint32_t) RFKeyCode::S, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location -= EntityManager::GetComponent<TransformComponent>(e).GetForward() * movementSpeed * TimeManager::GetDeltaTime();
            });

	    Entity cameraEntity = LevelManager::CreateEntity();
	    EntityManager::AddComponent<TransformComponent>(cameraEntity, transform);
	    EntityManager::AddComponent<CameraComponent>(cameraEntity, camera);
		EntityManager::AddComponent<InputComponent>(cameraEntity, input);
        CameraManager::SetMainCamera(cameraEntity);
    }

    while(!glfwWindowShouldClose(GraphicsSystem::GetWindow()))
    {
		glfwPollEvents();

	    timeManager.Update();
        inputSystem.Update();
        physicsSystem.Update();
        networkSystem.Update();
        graphics.Update();
    }

    Shutdown();
}

void Engine::Startup(bool shouldOverrideFramebuffer, unsigned int overrideExtentWidth, unsigned int overrideExtentHeight)
{
    if(isRunning)
		return;
	isRunning = true;

    std::cout << "---- RedForge Version 0.1 ----" << std::endl;

    timeManager.Startup();
    resourceManager.Startup();
    inputSystem.Startup();
    entityManager.Startup();
    graphics.Startup(shouldOverrideFramebuffer, overrideExtentWidth, overrideExtentHeight);
    cameraManager.Startup();
    debugManager.Startup();
    physicsSystem.Startup();
	fileManager.Startup();
	levelManager.Startup();
	networkSystem.Startup();
}
void Engine::Shutdown()
{
    // Wait for device to finish operations before exiting
    vkDeviceWaitIdle(GraphicsSystem::GetDevice());

    networkSystem.Shutdown();
	levelManager.Shutdown();
	fileManager.Shutdown();
    physicsSystem.Shutdown();
    debugManager.Shutdown();
    cameraManager.Shutdown();
    entityManager.Shutdown();
    inputSystem.Shutdown();
    resourceManager.Shutdown();
    graphics.Shutdown();
    timeManager.Shutdown();

    isRunning = false;
}

void Engine::Update()
{
    glfwPollEvents();

    timeManager.Update();
    inputSystem.Update();
    physicsSystem.Update();
    networkSystem.Update();
    graphics.Update();

    //EntityManager::GetComponent<TransformComponent>(0).rotation = glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(1, 0, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 1, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 0, 1)) * EntityManager::GetComponent<TransformComponent>(0).rotation;

    DebugManager::DrawDebugBox(
        glm::vec3(2, 1, 1), glm::angleAxis(glm::radians(60.0f * (float) TimeManager::GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    DebugManager::DrawDebugSphere(
        glm::vec3(2, 3, 1), glm::angleAxis(glm::radians(60.0f * (float) TimeManager::GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    LevelManager::ForEachEntity([this](const Entity& entity)
        {
            if(EntityManager::HasComponent<TransformComponent>(entity))
            {
			    //EntityManager::GetComponent<TransformComponent>(e).location += glm::vec3(0.0f, 0.0f, 1.0f) * TimeManager::GetDeltaTime();
                //EntityManager::GetComponent<TransformComponent>(e).rotation *= glm::angleAxis(glm::radians(60.0f * TimeManager::GetDeltaTime()), glm::normalize(glm::vec3(0, 0, 1)));
                //EntityManager::GetComponent<TransformComponent>(e).rotation = glm::angleAxis(glm::radians(60.0f * TimeManager::GetDeltaTime()), glm::normalize(glm::vec3(0, 0, 1))) * EntityManager::GetComponent<TransformComponent>(e).rotation;
			    //EntityManager::GetComponent<TransformComponent>(e).scale += glm::vec3(-0.01f, -0.01f, -0.01f) * TimeManager::GetDeltaTime();
            }
        });
}