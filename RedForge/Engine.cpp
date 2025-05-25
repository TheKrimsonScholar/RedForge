#include "Engine.h"

#include <iostream>

#include "TransformComponent.h"
#include "InputComponent.h"

void Engine::Run()
{
    if(isRunning)
		return;
	isRunning = true;

    timeManager.Startup();
	resourceManager.Startup();
	inputSystem.Startup();
    entityManager.Startup();
    graphics.Startup();
	cameraManager.Startup();

    {
        TransformComponent transform{};
        transform.location = { 1, 0, 0 };
        transform.rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		transform.scale = { 0.25f, 0.25f, 0.25f };

        MeshRendererComponent renderer{};
        renderer.UseMeshDefaults(L"KhaimBook.obj");

        Entity entity = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(entity, transform);
        EntityManager::AddComponent<MeshRendererComponent>(entity, renderer);
    }

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
        input.mouseDownCallbacks.emplace(GLFW_MOUSE_BUTTON_1, [lookSpeed](Entity e)
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
        input.keyDownCallbacks.emplace(GLFW_KEY_D, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location += EntityManager::GetComponent<TransformComponent>(e).GetRight() * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace(GLFW_KEY_A, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location -= EntityManager::GetComponent<TransformComponent>(e).GetRight() * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace(GLFW_KEY_SPACE, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location += glm::vec3(0, 1, 0) * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace(GLFW_KEY_LEFT_SHIFT, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location -= glm::vec3(0, 1, 0) * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace(GLFW_KEY_W, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location += EntityManager::GetComponent<TransformComponent>(e).GetForward() * movementSpeed * TimeManager::GetDeltaTime();
            });
        input.keyDownCallbacks.emplace(GLFW_KEY_S, [movementSpeed](Entity e)
            {
                EntityManager::GetComponent<TransformComponent>(e).location -= EntityManager::GetComponent<TransformComponent>(e).GetForward() * movementSpeed * TimeManager::GetDeltaTime();
            });

	    Entity cameraEntity = EntityManager::CreateEntity();
	    EntityManager::AddComponent<TransformComponent>(cameraEntity, transform);
	    EntityManager::AddComponent<CameraComponent>(cameraEntity, camera);
		EntityManager::AddComponent<InputComponent>(cameraEntity, input);
        CameraManager::SetMainCamera(cameraEntity);
    }

    {
        TransformComponent transform{};
        transform.location = { 0, -1, 0 };
        transform.rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        transform.scale = { 5.0f, 5.0f, 5.0f };

        MeshRendererComponent renderer{};
        renderer.UseMeshDefaults(L"primitives\\quad.obj");

        Entity plane = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(plane, transform);
        EntityManager::AddComponent<MeshRendererComponent>(plane, renderer);
    }

    while(!glfwWindowShouldClose(GraphicsSystem::GetWindow()))
    {
		glfwPollEvents();

	    timeManager.Update();
        inputSystem.Update();
        graphics.Update();

		EntityManager::GetComponent<TransformComponent>(0).rotation = glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(1, 0, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 1, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 0, 1)) * EntityManager::GetComponent<TransformComponent>(0).rotation;

        for(Entity e = 0; e < EntityManager::GetLastEntity(); e++)
            if(EntityManager::HasComponent<TransformComponent>(e))
            {
				//EntityManager::GetComponent<TransformComponent>(e).location += glm::vec3(0.0f, 0.0f, 1.0f) * TimeManager::GetDeltaTime();
                //EntityManager::GetComponent<TransformComponent>(e).rotation *= glm::angleAxis(glm::radians(60.0f * TimeManager::GetDeltaTime()), glm::normalize(glm::vec3(0, 0, 1)));
                //EntityManager::GetComponent<TransformComponent>(e).rotation = glm::angleAxis(glm::radians(60.0f * TimeManager::GetDeltaTime()), glm::normalize(glm::vec3(0, 0, 1))) * EntityManager::GetComponent<TransformComponent>(e).rotation;
				//EntityManager::GetComponent<TransformComponent>(e).scale += glm::vec3(-0.01f, -0.01f, -0.01f) * TimeManager::GetDeltaTime();
            }
    }

    // Wait for device to finish operations before exiting
    vkDeviceWaitIdle(GraphicsSystem::GetDevice());

    cameraManager.Shutdown();
    entityManager.Shutdown();
	inputSystem.Shutdown();
	resourceManager.Shutdown();
    graphics.Shutdown();
    timeManager.Shutdown();

	isRunning = false;
}