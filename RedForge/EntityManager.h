#pragma once

#include <unordered_map>
#include <any>
#include <queue>
#include <cassert>
#include <iostream>

#include "ComponentMacros.h"

#include "Exports.h"

static const uint32_t INVALID_ENTITY = 0xFFFFFFFF;
static const uint32_t MAX_ENTITIES = 256;

struct Entity
{
public:
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
	template<>
	struct hash<Entity>
	{
		size_t operator()(const Entity& e) const
		{
			std::hash<uint32_t> hasher;
			size_t seed = hasher(e.index);
			seed ^= hasher(e.generation) + 0x9e3779b9 + (seed << 6) + (seed >> 2); // 0x9e3779b9 is inverse golden ratio

			return seed;
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

	virtual void Resize(size_t size) = 0;

	virtual uint32_t GetComponentIndex(uint32_t entity) = 0;
	virtual void* GetRaw(size_t stride, uint32_t entity) = 0;

	virtual void* GetRawArray() = 0;
	virtual uint32_t* GetEntitiesArray() = 0;

	virtual uint32_t GetSize() = 0;
};
template<typename T>
class ComponentArray : public IComponentArray
{
private:
	// A contiguous array of all active components of this type.
	std::vector<T> components;
	// The entity index corresponding to each active component.
	std::vector<uint32_t> entityIndices;

	// Sparse array mapping entity indices to component array indices.
	std::vector<uint32_t> entityToComponentIndex;

public:
	ComponentArray();
	~ComponentArray() override;

	T* Add(uint32_t entity, T component);
	void* Add(uint32_t entity) override;
	void Remove(uint32_t entity) override;
	bool Has(uint32_t entity) override;
	T& GetByEntityIndex(uint32_t entity);
	T& GetByComponentIndex(uint32_t index);

	void Resize(size_t size) override;

	uint32_t GetComponentIndex(uint32_t entity) override;
	void* GetRaw(size_t stride, uint32_t entity) override;

	void* GetRawArray() override;
	uint32_t* GetEntitiesArray() override;

	uint32_t GetSize() override;
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
	// If entity already has a component of this type, just replace its data with the new data
	if(Has(entity))
	{
		uint32_t componentIndex = entityToComponentIndex[entity];
		
		components[componentIndex] = component;

		return &components[componentIndex];
	}

	/* Add the new component to the end of the components list */

	components.push_back(component);
	entityIndices.push_back(entity);
	entityToComponentIndex[entity] = components.size() - 1;

	return &components[components.size() - 1];
}
template<typename T>
inline void* ComponentArray<T>::Add(uint32_t entity)
{
	return Add(entity, T()); // Default construct the component
}
template<typename T>
void ComponentArray<T>::Remove(uint32_t entity)
{
	if(!Has(entity))
		return;

	uint32_t componentIndex = entityToComponentIndex[entity];
	uint32_t lastComponentIndex = components.size() - 1;
	uint32_t lastComponentEntityIndex = entityIndices[lastComponentIndex];
	// Swap the component to be removed with the last component in the array to maintain contiguousness
	std::swap(components[componentIndex], components[lastComponentIndex]);
	std::swap(entityIndices[componentIndex], entityIndices[lastComponentIndex]);
	std::swap(entityToComponentIndex[entity], entityToComponentIndex[lastComponentEntityIndex]);

	// Remove the last component in the array
	components.pop_back();
	entityIndices.pop_back();
	// Remove the entity's association with the component list
	entityToComponentIndex[entity] = -1;
}
template<typename T>
bool ComponentArray<T>::Has(uint32_t entity)
{
	return entityToComponentIndex[entity] != -1;
}
template<typename T>
T& ComponentArray<T>::GetByEntityIndex(uint32_t entity)
{
	assert(entityToComponentIndex[entity] != -1 && "Entity has no component of this type.");

	return components[entityToComponentIndex[entity]];
}
template<typename T>
inline T& ComponentArray<T>::GetByComponentIndex(uint32_t index)
{
	return components[index];
}

template<typename T>
inline void ComponentArray<T>::Resize(size_t size)
{
	entityToComponentIndex.resize(size, -1);
}

template<typename T>
inline uint32_t ComponentArray<T>::GetComponentIndex(uint32_t entity)
{
	return entityToComponentIndex[entity];
}
template<typename T>
void* ComponentArray<T>::GetRaw(size_t stride, uint32_t entity)
{
	assert(entityToComponentIndex[entity] != -1 && "Entity has no component of this type.");

	// Cast the relevant component array to a plain byte array and manually calculate this component's offset using custom stride
	return static_cast<void*>(reinterpret_cast<std::byte*>(components.data()) + entityToComponentIndex[entity] * stride);
}

template<typename T>
inline void* ComponentArray<T>::GetRawArray()
{
	return components.data();
}
template<typename T>
inline uint32_t* ComponentArray<T>::GetEntitiesArray()
{
	return entityIndices.data();
}

template<typename T>
inline uint32_t ComponentArray<T>::GetSize()
{
	return components.size();
}

class EntityManager
{
private:
	REDFORGE_API static inline EntityManager* Instance;

	uint32_t lastEntity = 0;
	std::vector<bool> entityStates;
	std::vector<uint32_t> generationCounts;
	// Queue of free entity indices less than lastEntity, sorted by index
	std::priority_queue<uint32_t> freeQueue;

	std::unordered_map<std::type_index, IComponentArray*> componentArrays;

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
	template<typename... Args>
	static inline bool HasComponents(Entity entity);
	template<typename T>
	static inline T& GetComponent(Entity entity);

	REDFORGE_API static void* AddComponentOfType(Entity entity, std::type_index componentType);
	REDFORGE_API static void RemoveComponentOfType(Entity entity, std::type_index componentType);
	REDFORGE_API static bool HasComponentOfType(Entity entity, std::type_index componentType);

	template<typename T>
	// Efficiently iterates through all active components of the specified type, triggering the given callback for each.
	REDFORGE_API static inline void ForEachComponentOfType(std::function<void(const Entity&, T&)> callback);
	template<typename... Args, typename Callback>
	// Efficiently iterates through all active entities which have all of the specified component types. Triggers the callback for each, providing the entity along with each of its relevant components by reference.
	REDFORGE_API static inline void ForEachComponentOfType(Callback&& callback);

	REDFORGE_API static std::unordered_map<void*, std::type_index> GetAllComponents(Entity entity);

	REDFORGE_API static bool IsEntityValid(Entity entity);
	REDFORGE_API static bool IsComponentValid(Entity entity, std::type_index componentType);

private:
	template<typename... Components>
	static inline Entity CreateEntity();
	static void DestroyEntity(Entity entity);

	REDFORGE_API static Entity GetEntityByIndex(uint32_t index);

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
template<typename... Args>
inline bool EntityManager::HasComponents(Entity entity)
{
	return (HasComponent<Args>(entity) && ...);
}
template<typename T>
inline T& EntityManager::GetComponent(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	assert(Instance->componentArrays.find(typeid(T)) != Instance->componentArrays.end() && "Component has not been registered.");

	return static_cast<ComponentArray<T>*>(Instance->componentArrays[typeid(T)])->GetByEntityIndex(entity.index);
}

template<typename... Components>
inline Entity EntityManager::CreateEntity()
{
	/* Determine next entity index */

	Entity entity = {};
	if(!Instance->freeQueue.empty())
	{
		entity.index = Instance->freeQueue.top();
		Instance->freeQueue.pop();
	}
	else
	{
		entity.index = Instance->lastEntity++;
		Instance->entityStates.resize(Instance->lastEntity);
		Instance->generationCounts.resize(Instance->lastEntity);
		// Resize all component array index maps to fit the new entity
		for(const std::pair<std::type_index, IComponentArray*>& componentArray : Instance->componentArrays)
			componentArray.second->Resize(Instance->lastEntity);
	}

	Instance->entityStates[entity.index] = true;
	entity.generation = ++Instance->generationCounts[entity.index];

	// Add all default components
	(AddComponent<Components>(entity, {}), ...);

	return entity;
}

template<typename T>
inline void EntityManager::ForEachComponentOfType(std::function<void(const Entity&, T&)> callback)
{
	IComponentArray* componentArray = Instance->componentArrays[typeid(T)];

	T* components = reinterpret_cast<T*>(componentArray->GetRawArray());
	uint32_t* entities = componentArray->GetEntitiesArray();
	uint32_t count = componentArray->GetSize();

	// Trigger callback for all components
	for(uint32_t i = 0; i < count; i++)
		callback(GetEntityByIndex(entities[i]), components[i]);
}
template<typename... Args, typename Callback>
inline void EntityManager::ForEachComponentOfType(Callback&& callback)
{
	std::vector<std::type_index> componentTypes = { (std::type_index(typeid(Args)))... };

	IComponentArray* smallestArray = nullptr;
	uint32_t smallestArraySize = -1;
	// Determine the smallest component array
	for(const std::type_index& type : componentTypes)
	{
		IComponentArray* componentArray = Instance->componentArrays[type];
		if(componentArray->GetSize() < smallestArraySize)
		{
			smallestArray = componentArray;
			smallestArraySize = componentArray->GetSize();
		}
	}

	uint32_t* entitiesArray = smallestArray->GetEntitiesArray();
	// Iterate through the the smallest array, checking which of the associated entities have all of the specified components
	for(uint32_t i = 0; i < smallestArraySize; i++)
	{
		uint32_t entityIndex = entitiesArray[i];
		const Entity& entity = GetEntityByIndex(entityIndex);

		std::tuple<ComponentArray<Args>*...> componentArrays = std::make_tuple(static_cast<ComponentArray<Args>*>(Instance->componentArrays[typeid(Args)])...);
		std::tuple<Args*...> componentPtrs;
		// Use index sequence to check for the other component types
		bool hasRequiredComponents = [&]<size_t... I>(std::index_sequence<I...>) -> bool
		{
			return (
				// If hasRequiredComponents is set to false (we find a component that doesn't exist on the entity), stop iteration and skip entity
				(
					[&]() -> bool
					{
						auto* componentArray = std::get<I>(componentArrays);
						// If componentArray is the smallest array (the one being iterated through), then we know the component exists for this entity
						if(componentArray == smallestArray)
						{
							// Set the component pointer at this index
							std::get<I>(componentPtrs) = &componentArray->GetByComponentIndex(i);
						}
						else
						{
							uint32_t otherComponentIndex = componentArray->GetComponentIndex(entityIndex);
							// If the entity doesn't have this component, stop and skip this entity
							if(otherComponentIndex == -1)
								return false;

							// Set the component pointer at this index
							std::get<I>(componentPtrs) = &componentArray->GetByComponentIndex(otherComponentIndex);
						}

						return true;
					}())
			&& ...);
		}(std::make_index_sequence<sizeof...(Args)>{});

		if(hasRequiredComponents)
		{
			std::apply(
				[&](Args*... components)
				{
					callback(entity, *components...);
				},
				componentPtrs);
		}





		//// Use index sequence and fold expression for O(1) checks/access
		//[&]<size_t... I>(std::index_sequence<I...>)
		//{
		//	// If hasRequiredComponents is set to false (we find a component that doesn't exist on the entity), stop iteration and skip entity
		//	(hasRequiredComponents && [&]
		//	{
		//		auto* componentArray = std::get<I>(componentArrays);
		//		// If componentArray is the smallest array (the one being iterated through), then we know the component exists for this entity
		//		if(componentArray == smallestArray)
		//		{
		//			// Set the component pointer at this index
		//			std::get<I>(componentPtrs) = &componentArray->GetByComponentIndex(i);
		//		}
		//		else
		//		{
		//			uint32_t otherComponentIndex = componentArray->GetComponentIndex(entityIndex);
		//			// If the entity doesn't have this component, stop
		//			if(otherComponentIndex == -1)
		//				hasRequiredComponents = false;

		//			// Set the component pointer at this index
		//			std::get<I>(componentPtrs) = &componentArray->GetByComponentIndex(otherComponentIndex);
		//		}
		//	}(), ...);
		//}(std::make_index_sequence<sizeof...(Args)>{});

		/*[&]()
		{
			[&] <size_t... I>(std::index_sequence<I...>)
			{
				(
					(hasRequiredComponents &&
						[&, I]
						{
							int k = (int) I;
							return k >= 0;
						}())
				, ...);

			}(std::make_index_sequence<sizeof...(Args)>{});
		}();*/






		//// This lambda will store the retrieved component references
		//auto componentRefPack =	
		//	[&hasRequiredComponents, entityID, i, &componentArrays, &entity](auto&&... refs)
		//	{
		//		// Use an index sequence to iterate over the pack and access the tuples
		//		([&] <size_t I> (auto& ref)
		//		{
		//			auto* componentArray = std::get<I>(componentArrays);
		//			componentArray->GetByComponentIndex(otherComponentIndex.second);
		//		}(refs), ...);

		//	};
		
		
		
		
		//// If the entity has all required components, trigger the callback, passing relevant components as parameters
		//if(HasComponents<Args...>(entity))
		//{
		//	std::tuple<Args&...> componentsTuple = std::make_tuple(std::ref(GetComponent<Args>(entity))...);
		//	std::apply(
		//		[&](Args&... components)
		//		{
		//			callback(entity, components...);
		//		}, componentsTuple);
		//}
	}
}