#pragma once

#include <unordered_map>
#include <any>
#include <queue>
#include <cassert>
#include <iostream>

#include "ComponentMacros.h"

#include "Exports.h"

//using Entity = uint32_t;

static const uint32_t INVALID_ENTITY = 0xFFFFFFFF;
static const uint32_t MAX_ENTITIES = 256;

struct Entity
{
private:
	uint32_t index = INVALID_ENTITY;
	uint32_t generation = INVALID_ENTITY;

	// Entity data should only be managed by EntityManager and LevelManager
	friend class EntityManager;
	friend class LevelManager;

	friend struct std::formatter<Entity, char>;
	friend struct std::hash<Entity>;

public:
	bool operator==(const Entity& other) const { return index == other.index && generation == other.generation; }
	bool operator!=(const Entity& other) const { return !(*this == other); }

	bool IsValid() const { return index != INVALID_ENTITY && generation != INVALID_ENTITY; }
};
template <>
struct std::formatter<Entity, char>
{
	constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const Entity& e, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "Entity(index={}, generation={})", e.index, e.generation);
	}
};
namespace std
{
	template<> struct hash<Entity>
	{
		size_t operator()(const Entity& e) const
		{
			return hash<float>()(e.index) ^ hash<float>()(e.generation);
		}
	};
}

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;

	virtual void* Add(uint32_t entity) = 0;
	virtual void Remove(uint32_t entity) = 0;
	virtual bool Has(uint32_t entity) = 0;

	virtual void* GetRaw(size_t stride, uint32_t entity) = 0;
};
template<typename T>
class ComponentArray : public IComponentArray
{
private:
	uint32_t lastComponentIndex = 0;
	// Queue of free component indices less than lastComponentIndex, sorted by index
	std::priority_queue<uint32_t> freeQueue;

	T components[MAX_ENTITIES] = {};
	std::unordered_map<uint32_t, uint32_t> entityToComponentIndex;

	size_t componentStride;

public:
	ComponentArray();
	~ComponentArray() override;

	T* Add(uint32_t entity, T component);
	void* Add(uint32_t entity) override;
	void Remove(uint32_t entity) override;
	bool Has(uint32_t entity) override;
	T& Get(uint32_t entity);

	void* GetRaw(size_t stride, uint32_t entity) override;
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
T* ComponentArray<T>::Add(uint32_t entity, T component)
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

	return &components[componentIndex];
}
template<typename T>
inline void* ComponentArray<T>::Add(uint32_t entity)
{
	return Add(entity, T()); // Default construct the component
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

template<typename T>
void* ComponentArray<T>::GetRaw(size_t stride, uint32_t entity)
{
	assert(entityToComponentIndex.find(entity) != entityToComponentIndex.end() && "Entity has no component of this type.");

	// Cast the relevant component array to a plain byte array and manually calculate this component's offset using custom stride
	return static_cast<void*>(reinterpret_cast<std::byte*>(components) + entityToComponentIndex[entity] * stride);
}

class EntityManager
{
private:
	REDFORGE_API static inline EntityManager* Instance;

	uint32_t lastEntity = 0;
	bool entityStates[MAX_ENTITIES] = {};
	// Queue of free entity indices less than lastEntity, sorted by index
	std::priority_queue<uint32_t> freeQueue;
	uint32_t generationCounts[MAX_ENTITIES] = {};

	std::unordered_map<std::type_index, IComponentArray*> componentArrays;

	//std::unordered_map<std::type_index, ComponentID> typeToComponentID;
	std::unordered_map<std::type_index, size_t> registeredComponentSizes;

public:
	EntityManager() {};
	~EntityManager() {};

	void Startup();
	void Shutdown();

	static inline uint32_t GetLastEntity() { return Instance->lastEntity; };

	template<typename T>
	static inline void RegisterComponent();
	template<typename T>
	static inline void AddComponent(Entity entity, T component);
	template<typename T>
	static inline void RemoveComponent(Entity entity);
	template<typename T>
	static inline bool HasComponent(Entity entity);
	template<typename T>
	static inline T& GetComponent(Entity entity);

	REDFORGE_API static void* AddComponentOfType(Entity entity, std::type_index componentType);
	REDFORGE_API static void RemoveComponentOfType(Entity entity, std::type_index componentType);
	REDFORGE_API static bool HasComponentOfType(Entity entity, std::type_index componentType);

	REDFORGE_API static std::unordered_map<void*, std::type_index> GetAllComponents(Entity entity);

	REDFORGE_API static bool IsEntityValid(Entity entity);
	REDFORGE_API static bool IsComponentValid(Entity entity, std::type_index componentType);

private:
	static Entity CreateEntity();
	static void DestroyEntity(Entity entity);

	static Entity GetEntityByIndex(uint32_t index);

	friend class LevelManager;
};

template<typename T>
inline void EntityManager::RegisterComponent()
{
	Instance->componentArrays[typeid(T)] = new ComponentArray<T>();
	Instance->registeredComponentSizes[typeid(T)] = sizeof(T);
}
template<typename T>
inline void EntityManager::AddComponent(Entity entity, T component)
{
	assert(IsEntityValid(entity) && "Attempting to modify invalid entity.");

	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");

	// First add all components that the given component depends on
	for(std::type_index dependencyID : GET_COMPONENT_DEPENDENCIES(typeid(T)))
		if(!HasComponentOfType(entity, dependencyID))
			AddComponentOfType(entity, dependencyID);

	static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->Add(entity.index, component);
}
template<typename T>
inline void EntityManager::RemoveComponent(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to modify invalid entity.");

	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");

	static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->Remove(entity.index);

	// After removing the component, also remove all components that depend on it
	for(std::type_index dependent : GET_COMPONENT_DEPENDENTS(typeid(T)))
		if(HasComponentOfType(entity, dependent))
			RemoveComponentOfType(entity, dependent);
}
template<typename T>
inline bool EntityManager::HasComponent(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");

	return static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->Has(entity.index);
}
template<typename T>
inline T& EntityManager::GetComponent(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");

	return static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->Get(entity.index);
}