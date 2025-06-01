#pragma once

#include <unordered_map>
#include <any>
#include <typeindex>
#include <queue>
#include <cassert>

#include "Exports.h"

using Entity = uint32_t;

static const uint32_t MAX_ENTITIES = 256;

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;

	virtual void Remove(uint32_t entity) = 0;
	virtual bool Has(uint32_t entity) = 0;
};
template<typename T>
class ComponentArray : public IComponentArray
{
private:
	uint32_t lastComponentIndex = 0;
	// Queue of free component indices less than lastComponentIndex, sorted by index
	std::priority_queue<uint32_t> freeQueue;

	T components[MAX_ENTITIES] = {};
	std::unordered_map<Entity, uint32_t> entityToComponentIndex;

public:
	ComponentArray();
	~ComponentArray() override;

	void Add(uint32_t entity, T component);
	void Remove(uint32_t entity) override;
	bool Has(uint32_t entity) override;
	T& Get(uint32_t entity);
};

template<typename T>
inline ComponentArray<T>::ComponentArray()
{
	
}
template<typename T>
inline ComponentArray<T>::~ComponentArray()
{
	
}

template<typename T>
void ComponentArray<T>::Add(uint32_t entity, T component)
{
	/* Determine next component index */

	uint32_t componentIndex;
	if(!freeQueue.empty())
	{
		componentIndex = freeQueue.top();
		freeQueue.pop();
	}
	else
		componentIndex = lastComponentIndex++;

	components[componentIndex] = component;
	entityToComponentIndex[entity] = componentIndex;
}
template<typename T>
void ComponentArray<T>::Remove(uint32_t entity)
{
	uint32_t componentIndex = entityToComponentIndex[entity];

	// If the component has the highest index of all valid components, "shrink" the valid range
	if(componentIndex == lastComponentIndex - 1)
	{
		lastComponentIndex--;
		// Keep moving lastComponentIndex down until we hit a non-empty index
		while(!freeQueue.empty() && lastComponentIndex - 1 == freeQueue.top())
		{
			freeQueue.pop();
			lastComponentIndex--;
		}
	}
	// Otherwise the component is in the valid range, so add it to free queue
	else
		freeQueue.push(componentIndex);

	entityToComponentIndex.erase(entity);
}
template<typename T>
bool ComponentArray<T>::Has(uint32_t entity)
{
	return entityToComponentIndex.find(entity) != entityToComponentIndex.end();
}
template<typename T>
T& ComponentArray<T>::Get(uint32_t entity)
{
	assert(entityToComponentIndex.find(entity) != entityToComponentIndex.end() && "Entity has no component of this type.");

	return components[entityToComponentIndex[entity]];
}

REDFORGE_API class EntityManager
{
private:
	static inline REDFORGE_API EntityManager* Instance;

	Entity lastEntity = 0;
	// Queue of free entity indices less than lastEntity, sorted by index
	std::priority_queue<Entity> freeQueue;
	uint32_t generationCounts[MAX_ENTITIES] = {};

	std::unordered_map<std::type_index, IComponentArray*> componentArrays;

public:
	EntityManager() {};
	~EntityManager() {};

	void Startup();
	void Shutdown();

	static REDFORGE_API Entity CreateEntity();
	static inline void DestroyEntity(Entity entity);

	static inline Entity GetLastEntity() { return Instance->lastEntity; };

	template<typename T>
	inline void RegisterComponent();
	template<typename T>
	static inline REDFORGE_API void AddComponent(uint32_t entity, T component);
	template<typename T>
	static inline void RemoveComponent(uint32_t entity);
	template<typename T>
	static inline bool HasComponent(uint32_t entity);
	template<typename T>
	static inline T& GetComponent(uint32_t entity);
};

template<typename T>
inline void EntityManager::RegisterComponent()
{
	Instance->componentArrays[typeid(T)] = new ComponentArray<T>();
}
template<typename T>
inline void EntityManager::AddComponent(uint32_t entity, T component)
{
	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");
	static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->Add(entity, component);
}
template<typename T>
inline void EntityManager::RemoveComponent(uint32_t entity)
{
	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");
	static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->Remove(entity);
}
template<typename T>
inline bool EntityManager::HasComponent(uint32_t entity)
{
	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");
	return static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->Has(entity);
}
template<typename T>
inline T& EntityManager::GetComponent(uint32_t entity)
{
	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");
	return static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->Get(entity);
}