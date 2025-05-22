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

    {
        TransformComponent transform{};
        transform.degrees = 60;

        MeshRendererComponent renderer{};
        renderer.mesh = ResourceManager::GetMesh(0);
        renderer.material = ResourceManager::GetMaterial(0);

        Entity entity1 = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(entity1, transform);
        EntityManager::AddComponent<MeshRendererComponent>(entity1, renderer);
    }

    {
        TransformComponent transform{};
        transform.degrees = 30;

        MeshRendererComponent renderer{};
        renderer.mesh = ResourceManager::GetMesh(1);
        renderer.material = ResourceManager::GetMaterial(1);

        Entity entity1 = EntityManager::CreateEntity();
        EntityManager::AddComponent<TransformComponent>(entity1, transform);
        EntityManager::AddComponent<MeshRendererComponent>(entity1, renderer);
    }

    {
        TransformComponent transform{};
        transform.degrees = 0;

        MeshRendererComponent renderer{};
        renderer.mesh = ResourceManager::GetMesh(0);
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
                EntityManager::GetComponent<TransformComponent>(e).degrees += 0.1f;
    }

    // Wait for device to finish operations before exiting
    vkDeviceWaitIdle(GraphicsSystem::GetDevice());

    entityManager.Shutdown();
	resourceManager.Shutdown();
    graphics.Shutdown();
    timeManager.Shutdown();

	isRunning = false;
}