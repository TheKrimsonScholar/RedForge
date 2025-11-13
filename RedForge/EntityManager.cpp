#include "EntityManager.h"

#include "TransformComponent.h"
#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "InputComponent.h"
#include "ColliderComponent.h"
#include "PhysicsComponent.h"
#include "LightComponent.h"

#include <iostream>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

void EntityManager::Startup()
{
	Instance = this;

	// Register all components defined in the global list
	for(auto& componentInfo : GetRegisteredComponentInfoMap())
	{
		std::cout << "Name: " << componentInfo.first.name() << std::endl;
		assert(componentInfo.second.registerComponent != nullptr);
		componentInfo.second.registerComponent();
	}
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

	Entity entity = {};
	if(!Instance->freeQueue.empty())
	{
		entity.index = Instance->freeQueue.top();
		Instance->freeQueue.pop();
	}
	else
		entity.index = Instance->lastEntity++;

	Instance->entityStates[entity.index] = true;
	entity.generation = ++Instance->generationCounts[entity.index];

	return entity;
}
void EntityManager::DestroyEntity(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to destroy invalid entity.");

	// If the entity has the highest index of all valid entities, "shrink" the valid range
	if(entity.index == Instance->lastEntity - 1)
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
		Instance->freeQueue.push(entity.index);

	// Remove all of the entity's components (prevents conflict if this entity slot is reused)
	for(auto& componentArray : Instance->componentArrays)
		componentArray.second->Remove(entity.index);

	Instance->entityStates[entity.index] = false;
}

Entity EntityManager::GetEntityByIndex(uint32_t index)
{
	Entity entity = {};
	entity.index = index;
	entity.generation = index == INVALID_ENTITY ? INVALID_ENTITY : Instance->generationCounts[index];

	return entity;
}

void* EntityManager::AddComponentOfType(Entity entity, std::type_index componentType)
{
	assert(IsEntityValid(entity) && "Attempting to modify invalid entity.");

	assert(Instance->componentArrays.find(componentType) != Instance->componentArrays.end() && "Component has not been registered.");

	// First add all components that the given component depends on
	for(std::type_index dependencyID : GET_COMPONENT_DEPENDENCIES(componentType))
		if(!HasComponentOfType(entity, dependencyID))
			AddComponentOfType(entity, dependencyID);

	return Instance->componentArrays[componentType]->Add(entity.index);
}
void EntityManager::RemoveComponentOfType(Entity entity, std::type_index componentType)
{
	assert(IsEntityValid(entity) && "Attempting to modify invalid entity.");

	assert(Instance->componentArrays.find(componentType) != Instance->componentArrays.end() && "Component has not been registered.");

	Instance->componentArrays[componentType]->Remove(entity.index);

	// After removing the component, also remove all components that depend on it
	for(std::type_index dependent : GET_COMPONENT_DEPENDENTS(componentType))
		if(HasComponentOfType(entity, dependent))
			RemoveComponentOfType(entity, dependent);
}
bool EntityManager::HasComponentOfType(Entity entity, std::type_index componentType)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	assert(Instance->componentArrays.find(componentType) != Instance->componentArrays.end() && "Component has not been registered.");

	return Instance->componentArrays[componentType]->Has(entity.index);
}

std::unordered_map<void*, std::type_index> EntityManager::GetAllComponents(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	std::unordered_map<void*, std::type_index> components;
	
	for(auto& componentArray : Instance->componentArrays)
	{
		if(componentArray.second->Has(entity.index))
		{
			void* componentPtr = componentArray.second->GetRaw(Instance->registeredComponentSizes[componentArray.first], entity.index);

			components.emplace(componentPtr, componentArray.first);
		}
	}

	return components;
}

bool EntityManager::IsEntityValid(Entity entity)
{
	// Entity index must be active and of the same generation
	return entity.index != INVALID_ENTITY && Instance->entityStates[entity.index] && Instance->generationCounts[entity.index] == entity.generation;
}
bool EntityManager::IsComponentValid(Entity entity, std::type_index componentType)
{
	if(!Instance)
		return false;
	if(!EntityManager::IsEntityValid(entity))
		return false;

	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	// Component is invalid if it isn't registered
	if(Instance->componentArrays.find(componentType) == Instance->componentArrays.end())
		return false;

	return Instance->componentArrays[componentType]->Has(entity.index);
}