#include "EntityManager.h"

#include "TransformComponent.h"
#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "InputComponent.h"
#include "ColliderComponent.h"
#include "PhysicsComponent.h"
#include "LightComponent.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

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

	Instance = nullptr;
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

	Instance->entityStates[entity] = true;
	Instance->generationCounts[entity]++;

	return entity;
}
void EntityManager::DestroyEntity(Entity entity)
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

	Instance->entityStates[entity] = false;
}

void EntityManager::AddComponentOfType(uint32_t entity, std::type_index componentType)
{
	assert(Instance->componentArrays.find(componentType) != Instance->componentArrays.end() && "Component has not been registered.");
	Instance->componentArrays[componentType]->Add(entity);
}
void EntityManager::RemoveComponentOfType(uint32_t entity, std::type_index componentType)
{
	assert(Instance->componentArrays.find(componentType) != Instance->componentArrays.end() && "Component has not been registered.");
	Instance->componentArrays[componentType]->Remove(entity);
}

std::unordered_map<void*, std::type_index> EntityManager::GetAllComponents(uint32_t entity)
{
	std::unordered_map<void*, std::type_index> components;
	
	for(auto& componentArray : Instance->componentArrays)
	{
		if(componentArray.second->Has(entity))
		{
			void* componentPtr = componentArray.second->GetRaw(Instance->registeredComponentSizes[componentArray.first], entity);

			components.emplace(componentPtr, componentArray.first);
		}
	}

	return components;
}

bool EntityManager::IsEntityValid(uint32_t entity)
{
	return Instance->entityStates[entity];
}
bool EntityManager::IsComponentValid(uint32_t entity, std::type_index componentType)
{
	if(!Instance)
		return false;

	// Component is invalid if it isn't registered
	if(Instance->componentArrays.find(componentType) == Instance->componentArrays.end())
		return false;

	return Instance->componentArrays[componentType]->Has(entity);
}