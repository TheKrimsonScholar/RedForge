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
	debugManager.Startup();
    physicsSystem.Startup();

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

        ColliderComponent collider{};
        collider.colliderType = EColliderType::Box;
        collider.halfSize = glm::vec3(5.0f, 0.2f, 5.0f);
        collider.center = glm::vec3(0.0f, 0.0f, 0.0f);

        PhysicsComponent physics{};
        physics.gravity = glm::vec3(0, 0, 0);
        physics.mass = 1;
        physics.isStatic = true;

        Entity plane = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(plane, transform);
        EntityManager::AddComponent<MeshRendererComponent>(plane, renderer);
        EntityManager::AddComponent<ColliderComponent>(plane, collider);
        EntityManager::AddComponent<PhysicsComponent>(plane, physics);
    }

    {
        TransformComponent transform{};
        transform.location = { 0, -1, 0 };
        transform.rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        transform.scale = { 5.0f, 5.0f, 5.0f };

        LightComponent light{};
        light.lightType = ELightType::Directional;
        light.color = glm::vec3(0, 0, 1);
        light.intensity = 0.25f;
        light.direction = glm::vec3(-1, -1, 0);

        Entity directionalLight = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(directionalLight, transform);
        EntityManager::AddComponent<LightComponent>(directionalLight, light);
    }

    {
        TransformComponent transform{};
        transform.location = { 0, -1, 0 };
        transform.rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        transform.scale = { 5.0f, 5.0f, 5.0f };

        LightComponent light{};
        light.lightType = ELightType::Point;
        light.color = glm::vec3(1, 1, 1);
        light.intensity = 1;
        light.location = glm::vec3(2, 0, 0);
        light.range = 10;

        Entity pointLight = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(pointLight, transform);
        EntityManager::AddComponent<LightComponent>(pointLight, light);
    }

    DebugManager::DrawDebugBox(
        glm::vec3(2, 1, 1), glm::angleAxis(glm::radians(60.0f * (float) TimeManager::GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 10.0f);

    while(!glfwWindowShouldClose(GraphicsSystem::GetWindow()))
    {
		glfwPollEvents();

	    timeManager.Update();
        inputSystem.Update();
        graphics.Update();
        physicsSystem.Update();

		//EntityManager::GetComponent<TransformComponent>(0).rotation = glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(1, 0, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 1, 0)) * glm::angleAxis(1.0f * TimeManager::GetDeltaTime(), glm::vec3(0, 0, 1)) * EntityManager::GetComponent<TransformComponent>(0).rotation;

        DebugManager::DrawDebugBox(
            glm::vec3(2, 1, 1), glm::angleAxis(glm::radians(60.0f * (float) TimeManager::GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        DebugManager::DrawDebugSphere(
            glm::vec3(2, 3, 1), glm::angleAxis(glm::radians(60.0f * (float) TimeManager::GetCurrentTime()), glm::normalize(glm::vec3(1, 1, 1))), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

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