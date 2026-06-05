#pragma once

#include <unordered_map>
#include <any>
#include <queue>
#include <cassert>
#include <iostream>

#include "ComponentMacros.h"
#include "Event.h"
#include "FileManager.h"

#include "Exports.h"

typedef size_t LevelID;

static const uint32_t INVALID_ENTITY = 0xFFFFFFFF;
static const uint32_t MAX_ENTITIES = 256;

static const LevelID INVALID_LEVEL = -1;

struct Entity
{
public:
	uint32_t index = INVALID_ENTITY;
	// The generation of this entity, used to determine whether the entity is out-of-date.
	// If 0, the entity is transient, i.e. it is owned by a non-authoritative source (like a command buffer) and will be added to the entity manager as a new entity with a different permanent ID in the future.
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

	virtual void Add(uint32_t entity, const void* component = nullptr) = 0;
	virtual void Remove(uint32_t entity) = 0;
	virtual bool Has(uint32_t entity) = 0;

	virtual void Resize(size_t size) = 0;

	virtual uint32_t GetComponentIndex(uint32_t entity) = 0;
	virtual void* GetRaw(uint32_t entity) = 0;

	virtual void* GetRawArray() = 0;
	virtual uint32_t* GetEntitiesArray() = 0;

	virtual uint32_t GetSize() = 0;
};
template<typename T>
class ComponentArray : public IComponentArray
{
private:
	// A contiguous array of all active components of this type.
	std::vector<std::remove_const_t<T>> components;
	// The entity index corresponding to each active component.
	std::vector<uint32_t> entityIndices;

	// Sparse array mapping entity indices to component array indices.
	std::vector<uint32_t> entityToComponentIndex;

public:
	ComponentArray();
	~ComponentArray() override;

	void Add(uint32_t entity, T component);
	void Add(uint32_t entity, const void* component = nullptr) override;
	void Remove(uint32_t entity) override;
	bool Has(uint32_t entity) override;
	T& GetByEntityIndex(uint32_t entity);
	T& GetByComponentIndex(uint32_t index);

	void Resize(size_t size) override;

	uint32_t GetComponentIndex(uint32_t entity) override;
	void* GetRaw(uint32_t entity) override;

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
void ComponentArray<T>::Add(uint32_t entity, T component)
{
	// If entity already has a component of this type, just replace its data with the new data
	if(Has(entity))
	{
		uint32_t componentIndex = entityToComponentIndex[entity];
		
		components[componentIndex] = component;

		return;
	}

	/* Add the new component to the end of the components list */

	components.push_back(component);
	entityIndices.push_back(entity);
	entityToComponentIndex[entity] = components.size() - 1;
}
template<typename T>
inline void ComponentArray<T>::Add(uint32_t entity, const void* component)
{
	// If using an initializer pointer, cast and copy it
	if(component)
		Add(entity, *reinterpret_cast<const T*>(component));
	// Otherwise default construct the component
	else
		Add(entity, T());
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
void* ComponentArray<T>::GetRaw(uint32_t entity)
{
	assert(entityToComponentIndex[entity] != -1 && "Entity has no component of this type.");

	// Cast the relevant component array to a plain byte array and manually calculate this component's offset
	return static_cast<void*>(reinterpret_cast<std::byte*>(components.data()) + entityToComponentIndex[entity] * sizeof(T));
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

class World;

struct LevelData
{
	std::string name = "";
	Entity first = {};
};
struct EntityData
{
	bool valid = false;
	bool active = true;
	size_t generationCount = 0;
	std::string name = "";
	LevelID level = INVALID_LEVEL;
	std::filesystem::path prefabPath = {};
	Entity parent = {};
	Entity firstChild = {};
	Entity nextSibling = {};
	Entity lastSibling = {};
	size_t depth = 0;
};
class EntityManager
{
private:
	World* world;

	Event<const Entity&> onEntityCreated;
	Event<const Entity&> onEntityDestroyed;
	Event<const Entity&> onEntityLevelDataModified;
	Event<const Entity&, const Entity&> onEntityReparented;
	Event<const Entity&, const Entity&> onEntityMovedBefore;
	Event<const Entity&, const Entity&> onEntityMovedAfter;

	// The smallest entity index that is greater than all currently valid entity indices.
	size_t nextEntity = 0;

	std::unordered_map<LevelID, LevelData> levelData;
	std::vector<EntityData> entityData;
	
	// Queue of free entity indices less than nextEntity, sorted by index.
	std::priority_queue<uint32_t> freeQueue;

	std::unordered_map<std::type_index, IComponentArray*> componentArrays;

public:
	EntityManager(World* world) : world(world) {};
	~EntityManager() {};

	void Startup();
	void Shutdown();

	template<typename... Components>
	Entity CreateEntity(const std::string& name = "New Entity", LevelID level = INVALID_LEVEL, const std::filesystem::path& prefabPath = {});
	template<typename... Components>
	Entity CreateEntity(const Entity& parent, const std::string& name = "New Entity", const std::filesystem::path& prefabPath = {});
	REDFORGE_API void DestroyEntity(Entity entity);

	REDFORGE_API bool ReparentEntity(Entity entity, Entity newParent);
	REDFORGE_API bool MoveEntityBefore(Entity entity, Entity next);
	REDFORGE_API bool MoveEntityAfter(Entity entity, Entity previous);

	uint32_t GetLastEntity() { return nextEntity; };

	template<typename T>
	void RegisterComponent();
	template<typename T>
	void AddComponent(Entity entity, T component);
	template<typename T>
	void RemoveComponent(Entity entity);
	template<typename T>
	bool HasComponent(Entity entity);
	template<typename... Args>
	bool HasComponents(Entity entity);
	template<typename T>
	T& GetComponent(Entity entity);

	REDFORGE_API void AddComponentOfType(Entity entity, std::type_index componentType, const void* component = nullptr);
	REDFORGE_API void RemoveComponentOfType(Entity entity, std::type_index componentType);
	REDFORGE_API bool HasComponentOfType(Entity entity, std::type_index componentType);
	REDFORGE_API void* GetComponentOfType(Entity entity, std::type_index componentType);

	// Recursively traverses the entity hierarchy of every active level, performing the callback on each valid entity.
	REDFORGE_API void ForEachEntity(std::function<void(const Entity&)> callback) const;
	// Recursively traverses level's entity hierarchy, performing the callback on each valid entity.
	REDFORGE_API void ForEachEntity(std::function<void(const Entity&)> callback, LevelID level) const;
	// Recursively traverses the sub-hierarchy below root (including root), performing the callback on each valid entity.
	REDFORGE_API void ForEachEntity(std::function<void(const Entity&)> callback, Entity root) const;
	// Recursively traverses the entity hierarchy of every active level, performing the callback on each valid entity in reverse order, ensuring all children are processed before their parents.
	REDFORGE_API void ForEachEntity_Reversed(std::function<void(const Entity&)> callback) const;
	// Recursively traverses level's entity hierarchy, performing the callback on each valid entity in reverse order, ensuring all children are processed before their parents.
	REDFORGE_API void ForEachEntity_Reversed(std::function<void(const Entity&)> callback, LevelID level) const;
	// Recursively traverses the sub-hierarchy below root (including root), performing the callback on each valid entity in reverse order, ensuring all children are processed before their parents.
	REDFORGE_API void ForEachEntity_Reversed(std::function<void(const Entity&)> callback, Entity root) const;

	template<typename T>
	// Efficiently iterates through all active components of the specified type, triggering the given callback for each.
	void ForEachComponentOfType(std::function<void(const Entity&, T&)> callback);
	template<typename... Args, typename Callback>
	// Efficiently iterates through all active entities which have all of the specified component types. Triggers the callback for each, providing the entity along with each of its relevant components by reference.
	void ForEachComponentOfType(Callback&& callback);

	REDFORGE_API std::unordered_map<void*, std::type_index> GetAllComponents(Entity entity);

	REDFORGE_API bool IsEntityValid(Entity entity) const;
	REDFORGE_API bool IsComponentValid(Entity entity, std::type_index componentType) const;

	REDFORGE_API bool IsEntityChildOf(Entity parent, Entity child) const;

	REDFORGE_API bool IsEntityActive(Entity entity) const;
	REDFORGE_API std::string GetEntityName(Entity entity) const;
	REDFORGE_API LevelID GetEntityLevel(Entity entity) const;
	REDFORGE_API std::filesystem::path GetEntityPrefabPath(Entity entity) const;
	REDFORGE_API Entity GetEntityParent(Entity entity) const;
	REDFORGE_API Entity GetEntityFirstChild(Entity entity) const;
	REDFORGE_API Entity GetEntityNextSibling(Entity entity) const;
	REDFORGE_API Entity GetEntityLastSibling(Entity entity) const;
	REDFORGE_API uint32_t GetEntityDepth(Entity entity) const;

	REDFORGE_API void SetEntityName(const Entity& entity, const std::string& name);

	REDFORGE_API void SaveEntityAsPrefab(const Entity& entity, const std::filesystem::path& prefabPath);
	REDFORGE_API Entity LoadEntityFromPrefab(const std::filesystem::path& prefabPath, const Entity& parent);

	REDFORGE_API void SaveLevel(const std::filesystem::path& levelPath);
	REDFORGE_API void LoadLevel(const std::filesystem::path& levelPath);

	Event<const Entity&>& GetOnEntityCreated() { return onEntityCreated; }
	Event<const Entity&>& GetOnEntityDestroyed() { return onEntityDestroyed; }
	Event<const Entity&>& GetOnEntityLevelDataModified() { return onEntityLevelDataModified; }
	Event<const Entity&, const Entity&>& GetOnEntityReparented() { return onEntityReparented; }
	Event<const Entity&, const Entity&>& GetOnEntityMovedBefore() { return onEntityMovedBefore; }
	Event<const Entity&, const Entity&>& GetOnEntityMovedAfter() { return onEntityMovedAfter; }

private:
	SerializedObject SaveEntity(Entity entity);
	Entity LoadEntity(const SerializedObject& entityObject, Entity parent);

	EntityData& GetEntityData(const Entity& entity);

	REDFORGE_API Entity GetEntityByIndex(uint32_t index);

	void SetEntityParent(Entity entity, Entity parent);
	void SetEntityFirstChild(Entity entity, Entity firstChild);
	void SetEntityNextSibling(Entity entity, Entity nextSibling);
	void SetEntityLastSibling(Entity entity, Entity lastSibling);

	friend class LevelManager;
};

template<typename... Components>
inline Entity EntityManager::CreateEntity(const std::string& name, LevelID level, const std::filesystem::path& prefabPath)
{
	/* Insert the new entity as parent's last child */
	
	Entity lastSibling = levelData[level].first;
	// Find the last child of the parent
	while(GetEntityNextSibling(lastSibling).IsValid())
		lastSibling = GetEntityNextSibling(lastSibling);
	
	/* Determine next entity index */

	Entity newEntity = {};
	if(!freeQueue.empty())
	{
		newEntity.index = freeQueue.top();
		freeQueue.pop();
	}
	else
	{
		newEntity.index = nextEntity++;
		entityData.resize(nextEntity);
		// Resize all component array index maps to fit the new entity
		for(const std::pair<std::type_index, IComponentArray*>& componentArray : componentArrays)
			componentArray.second->Resize(nextEntity);
	}

	entityData[newEntity.index].valid = true;
	newEntity.generation = ++entityData[newEntity.index].generationCount;

	// Add all default components
	(AddComponent<Components>(newEntity, {}), ...);

	/* Put entity in the level */

	EntityData data = {};
	data.valid = true;
	data.active = true;
	data.generationCount = newEntity.generation;
	data.name = name;
	data.level = level;
	data.prefabPath = prefabPath;
	data.parent = {};
	data.firstChild = {};
	data.nextSibling = {};
	data.lastSibling = lastSibling;
	data.depth = 1;

	// If the level is empty, add this as the first entity
	if(!levelData[level].first.IsValid())
		levelData[level].first = newEntity;
	// If there is a sibling before this entity, update it
	if(lastSibling.IsValid())
		entityData[lastSibling.index].nextSibling = newEntity;

	entityData[newEntity.index] = data;

	onEntityCreated.Broadcast(newEntity);

	return newEntity;
}
template<typename... Components>
inline Entity EntityManager::CreateEntity(const Entity& parent, const std::string& name, const std::filesystem::path& prefabPath)
{
	// If parent is invalid, create entity under the root of the persistent level
	if(!IsEntityValid(parent))
		return CreateEntity<Components...>(name, INVALID_LEVEL, prefabPath);

	/* Insert the new entity as parent's last child */

	Entity lastSibling = GetEntityFirstChild(parent);
	// Find the last child of the parent
	while(GetEntityNextSibling(lastSibling).IsValid())
		lastSibling = GetEntityNextSibling(lastSibling);
	
	/* Determine next entity index */

	Entity newEntity = {};
	if(!freeQueue.empty())
	{
		newEntity.index = freeQueue.top();
		freeQueue.pop();
	}
	else
	{
		newEntity.index = nextEntity++;
		entityData.resize(nextEntity);
		// Resize all component array index maps to fit the new entity
		for(const std::pair<std::type_index, IComponentArray*>& componentArray : componentArrays)
			componentArray.second->Resize(nextEntity);
	}

	entityData[newEntity.index].valid = true;
	newEntity.generation = ++entityData[newEntity.index].generationCount;

	// Add all default components
	(AddComponent<Components>(newEntity, {}), ...);

	/* Put entity in the level */

	EntityData data = {};
	data.valid = true;
	data.active = true;
	data.generationCount = newEntity.generation;
	data.name = name;
	data.level = GetEntityLevel(parent);
	data.prefabPath = prefabPath;
	data.parent = parent;
	data.firstChild = {};
	data.nextSibling = {};
	data.lastSibling = lastSibling;
	data.depth = GetEntityDepth(parent) + 1;

	// If parent doesn't have any children yet, this is the first child
	if(!GetEntityFirstChild(parent).IsValid())
		entityData[parent.index].firstChild = newEntity;
	// If there is a sibling before this entity, update it
	if(lastSibling.IsValid())
		entityData[lastSibling.index].nextSibling = newEntity;

	entityData[newEntity.index] = data;

	onEntityCreated.Broadcast(newEntity);

	return newEntity;
}

template<typename T>
inline void EntityManager::RegisterComponent()
{
	componentArrays[typeid(T)] = new ComponentArray<T>();
}
template<typename T>
inline void EntityManager::AddComponent(Entity entity, T component)
{
	assert(IsEntityValid(entity) && "Attempting to modify invalid entity.");

	assert(componentArrays.find(typeid(T)) != componentArrays.end() && "Component has not been registered.");

	// First add all components that the given component depends on
	for(std::type_index dependencyID : GET_COMPONENT_DEPENDENCIES(typeid(T)))
		if(!HasComponentOfType(entity, dependencyID))
			AddComponentOfType(entity, dependencyID);

	static_cast<ComponentArray<T>*>(componentArrays[typeid(T)])->Add(entity.index, component);
}
template<typename T>
inline void EntityManager::RemoveComponent(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to modify invalid entity.");

	assert(componentArrays.find(typeid(T)) != componentArrays.end() && "Component has not been registered.");

	static_cast<ComponentArray<T>*>(componentArrays[typeid(T)])->Remove(entity.index);

	// After removing the component, also remove all components that depend on it
	for(std::type_index dependent : GET_COMPONENT_DEPENDENTS(typeid(T)))
		if(HasComponentOfType(entity, dependent))
			RemoveComponentOfType(entity, dependent);
}
template<typename T>
inline bool EntityManager::HasComponent(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	assert(componentArrays.find(typeid(T)) != componentArrays.end() && "Component has not been registered.");

	return static_cast<ComponentArray<T>*>(componentArrays[typeid(T)])->Has(entity.index);
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

	assert(componentArrays.find(typeid(T)) != componentArrays.end() && "Component has not been registered.");

	return static_cast<ComponentArray<T>*>(componentArrays[typeid(T)])->GetByEntityIndex(entity.index);
}

template<typename T>
void EntityManager::ForEachComponentOfType(std::function<void(const Entity&, T&)> callback)
{
	IComponentArray* componentArray = componentArrays[typeid(T)];

	T* components = reinterpret_cast<T*>(componentArray->GetRawArray());
	uint32_t* entities = componentArray->GetEntitiesArray();
	uint32_t count = componentArray->GetSize();

	// Trigger callback for all components
	for(uint32_t i = 0; i < count; i++)
		callback(GetEntityByIndex(entities[i]), components[i]);
}
template<typename... Args, typename Callback>
void EntityManager::ForEachComponentOfType(Callback&& callback)
{
	std::vector<std::type_index> componentTypes = { (std::type_index(typeid(Args)))... };

	IComponentArray* smallestArray = nullptr;
	uint32_t smallestArraySize = -1;
	// Determine the smallest component array
	for(const std::type_index& type : componentTypes)
	{
		IComponentArray* componentArray = componentArrays[type];
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

		// TEMP
		//std::tuple<const int*> componentArrays = std::make_tuple(new int());
		//std::tuple<ComponentArray<Args>*...> componentArrays = std::make_tuple((new ComponentArray<Args>())...);
		
		std::tuple<ComponentArray<Args>*...> componentArraysTuple = std::make_tuple(static_cast<ComponentArray<Args>*>(componentArrays[typeid(Args)])...);
		std::tuple<Args*...> componentPtrs;
		// Use index sequence to check for the other component types
		bool hasRequiredComponents = [&]<size_t... I>(std::index_sequence<I...>) -> bool
		{
			return (
				// If hasRequiredComponents is set to false (we find a component that doesn't exist on the entity), stop iteration and skip entity
				(
					[&]() -> bool
					{
						auto* componentArray = std::get<I>(componentArraysTuple);
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
	}
}