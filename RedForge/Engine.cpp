#include "Engine.h"

#include "TransformComponent.h"

void Engine::Run()
{
    if(isRunning)
		return;
	isRunning = true;

    timeManager.Startup();
	resourceManager.Startup();
    entityManager.Startup();
    graphics.Startup();
	cameraManager.Startup();

    /*{
        TransformComponent transform{};
        transform.location = { 0, 0, 0 };
        transform.rotation = glm::angleAxis(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        transform.scale = { 1, 1, 1 };

        MeshRendererComponent renderer{};
        renderer.mesh = ResourceManager::GetMesh(0);
        renderer.material = ResourceManager::GetMaterial(0);

        Entity entity1 = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(entity1, transform);
        EntityManager::AddComponent<MeshRendererComponent>(entity1, renderer);
    }

    {
        TransformComponent transform{};
        transform.location = { 0, 0, 0 };
        transform.rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        transform.scale = { 1, 1, 1 };

        MeshRendererComponent renderer{};
        renderer.mesh = ResourceManager::GetMesh(1);
        renderer.material = ResourceManager::GetMaterial(1);

        Entity entity1 = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(entity1, transform);
        EntityManager::AddComponent<MeshRendererComponent>(entity1, renderer);
    }*/

    {
        TransformComponent transform{};
        transform.location = { 1, 0, 0 };
        transform.rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		transform.scale = { 0.25f, 0.25f, 0.25f };

        MeshRendererComponent renderer{};
        renderer.mesh = ResourceManager::GetMesh(1);
        renderer.material = ResourceManager::GetMaterial(1);

        Entity entity1 = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(entity1, transform);
        EntityManager::AddComponent<MeshRendererComponent>(entity1, renderer);
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

	    Entity cameraEntity = EntityManager::CreateEntity();
	    EntityManager::AddComponent<TransformComponent>(cameraEntity, transform);
	    EntityManager::AddComponent<CameraComponent>(cameraEntity, camera);
        CameraManager::SetMainCamera(cameraEntity);
    }

    while(!glfwWindowShouldClose(GraphicsSystem::GetWindow()))
    {
		glfwPollEvents();

	    timeManager.Update();
        graphics.Update();

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
	resourceManager.Shutdown();
    graphics.Shutdown();
    timeManager.Shutdown();

	isRunning = false;
}