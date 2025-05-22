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

    while(!glfwWindowShouldClose(GraphicsSystem::GetWindow()))
    {
		glfwPollEvents();

	    timeManager.Update();
        graphics.Update();

        for(Entity e = 0; e < EntityManager::GetLastEntity(); e++)
            if(EntityManager::HasComponent<TransformComponent>(e))
            {
				//EntityManager::GetComponent<TransformComponent>(e).location += glm::vec3(0.0f, 0.0f, 1.0f) * TimeManager::GetDeltaTime();
                EntityManager::GetComponent<TransformComponent>(e).rotation *= glm::angleAxis(glm::radians(60.0f * TimeManager::GetDeltaTime()), glm::normalize(glm::vec3(0, 0, 1)));
                //EntityManager::GetComponent<TransformComponent>(e).rotation = glm::angleAxis(glm::radians(60.0f * TimeManager::GetDeltaTime()), glm::normalize(glm::vec3(0, 0, 1))) * EntityManager::GetComponent<TransformComponent>(e).rotation;
				//EntityManager::GetComponent<TransformComponent>(e).scale += glm::vec3(-0.01f, -0.01f, -0.01f) * TimeManager::GetDeltaTime();
            }
    }

    // Wait for device to finish operations before exiting
    vkDeviceWaitIdle(GraphicsSystem::GetDevice());

    entityManager.Shutdown();
	resourceManager.Shutdown();
    graphics.Shutdown();
    timeManager.Shutdown();

	isRunning = false;
}