#include "EntityManager.h"

#include "TransformComponent.h"
#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "InputComponent.h"
#include "ColliderComponent.h"
#include "PhysicsComponent.h"
#include "LightComponent.h"

void EntityManager::Startup()
{
	Instance = this;

	RegisterComponent<TransformComponent>();
	RegisterComponent<MeshRendererComponent>();
	RegisterComponent<CameraComponent>();
	RegisterComponent<InputComponent>();
	RegisterComponent<ColliderComponent>();
	RegisterComponent<PhysicsComponent>();
	RegisterComponent<LightComponent>();
}
void EntityManager::Shutdown()
{
	for(auto& componentArray : componentArrays)
		delete componentArray.second;
}

Entity EntityManager::CreateEntity()
{
	/* Determine next entity index */

	Entity entity;
	if(!Instance->freeQueue.empty())
	{
		entity = Instance->freeQueue.top();
		Instance->freeQueue.pop();
	}
	else
		entity = Instance->lastEntity++;

	Instance->generationCounts[entity]++;

	return entity;
}
inline void EntityManager::DestroyEntity(Entity entity)
{
	// If the entity has the highest index of all valid entities, "shrink" the valid range
	if(entity == Instance->lastEntity - 1)
	{
		Instance->lastEntity--;
		// Keep moving lastEntity down until we hit a non-empty index
		while(!Instance->freeQueue.empty() && Instance->lastEntity - 1 == Instance->freeQueue.top())
		{
			Instance->freeQueue.pop();
			Instance->lastEntity--;
		}
	}
	// Otherwise the entity is in the valid range, so add it to free queue
	else
		Instance->freeQueue.push(entity);

	for(auto& componentArray : Instance->componentArrays)
		componentArray.second->Remove(entity);
}