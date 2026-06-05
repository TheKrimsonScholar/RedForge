#include "EntityManager.h"

#include "TransformComponent.h"
#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "InputComponent.h"
#include "ColliderComponent.h"
#include "PhysicsComponent.h"
#include "LightComponent.h"

#include <iostream>
#include <fstream>

#include "DebugMacros.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

void EntityManager::Startup()
{
	// Register all components defined in the global list
	for(auto& componentInfo : GetRegisteredComponentInfoMap())
	{
		std::cout << "Name: " << componentInfo.first.name() << std::endl;
		assert(componentInfo.second.registerComponent != nullptr);
		componentInfo.second.registerComponent(*this);
	}
}
void EntityManager::Shutdown()
{
	for(auto& componentArray : componentArrays)
		delete componentArray.second;
}

void EntityManager::DestroyEntity(Entity entity)
{
	// Silently return if entity is invalid or not present in level
	if(!IsEntityValid(entity))
		return;

	onEntityDestroyed.Broadcast(entity);

	/* Destroy all children first - gather list before destroying so hierarchy isn't destroyed while traversing it */

	std::vector<Entity> children;
	ForEachEntity([entity, &children](const Entity& child)
		{
			// Don't destroy this entity yet; only its children
			if(child == entity)
				return;

			children.push_back(child);
		}, entity);
	// Destroy the children
	for(const Entity& child : children)
		DestroyEntity(child);

	Entity parent = GetEntityParent(entity);
	Entity lastSibling = GetEntityLastSibling(entity);
	Entity nextSibling = GetEntityNextSibling(entity);
	
	// If this entity is the first child, set its next sibling as first child
	if(GetEntityFirstChild(parent) == entity)
		SetEntityFirstChild(parent, nextSibling);
	// If the last sibling exists, set its next sibling to the sibling after this entity
	SetEntityNextSibling(lastSibling, GetEntityNextSibling(entity));
	// If the next sibling exists, set its last sibling to the sibling before this entity
	SetEntityLastSibling(nextSibling, GetEntityLastSibling(entity));

	// Destroy entity
	// If the entity has the highest index of all valid entities, "shrink" the valid range
	if(entity.index == nextEntity - 1)
	{
		nextEntity--;
		// Keep moving lastEntity down until we hit a non-empty index
		while(!freeQueue.empty() && nextEntity - 1 == freeQueue.top())
		{
			freeQueue.pop();
			nextEntity--;
		}
	}
	// Otherwise the entity is in the valid range, so add it to free queue
	else
		freeQueue.push(entity.index);

	// Remove all of the entity's components (prevents conflict if this entity slot is reused)
	for(auto& componentArray : componentArrays)
		componentArray.second->Remove(entity.index);

	entityData[entity.index].valid = false;
}

bool EntityManager::ReparentEntity(Entity entity, Entity newParent)
{
	assert(entity.IsValid() && "Invalid entity.");

    // If trying to parent the entity to itself or its children, ignore
    if(IsEntityChildOf(entity, newParent))
        return false;

	onEntityReparented.Broadcast(entity, newParent);

    /* Update entity level data to set entity as the last child of newParent */

	// Update level's first entity if this is the first entity in the level
    if(levelData[entityData[entity.index].level].first == entity)
		levelData[entityData[entity.index].level].first = entityData[entity.index].nextSibling;
	// Update parent's firstChild if this is the first child
	if(GetEntityFirstChild(GetEntityParent(entity)) == entity)
		SetEntityFirstChild(GetEntityParent(entity), GetEntityNextSibling(entity));
    // Update last sibling's nextSibling if last sibling is valid
	SetEntityNextSibling(GetEntityLastSibling(entity), GetEntityNextSibling(entity));
    // Update next sibling's lastSibling if next sibling is valid
	SetEntityLastSibling(GetEntityNextSibling(entity), GetEntityLastSibling(entity));

	Entity lastSibling = GetEntityFirstChild(newParent);
    // Find last child of newParent
    while(GetEntityNextSibling(lastSibling).IsValid())
        lastSibling = GetEntityNextSibling(lastSibling);

    // If lastSibling is valid, set entity as the next child after it
	if(lastSibling.IsValid())
	{
		entityData[lastSibling.index].nextSibling = entity;
		onEntityLevelDataModified.Broadcast(lastSibling);
	}
    // Otherwise newParent has no children, so set entity as firstChild instead
    else
		entityData[newParent.index].firstChild = entity;

    // Update entity's level data
    entityData[entity.index].level = entityData[newParent.index].level;
    entityData[entity.index].parent = newParent;
    entityData[entity.index].nextSibling = {};
    entityData[entity.index].lastSibling = lastSibling;
    entityData[entity.index].depth = entityData[newParent.index].depth + 1;

	onEntityLevelDataModified.Broadcast(entity);

    return true;
}
bool EntityManager::MoveEntityBefore(Entity entity, Entity next)
{
	assert((entity.IsValid() && next.IsValid()) && "Invalid entities.");

    // If trying to move the entity to be a sibling of itself or one of its children, ignore
    if(IsEntityChildOf(entity, next))
        return false;

	onEntityMovedBefore.Broadcast(entity, next);

    /* Update entity level data to set entity as the last sibling before next */

	// Update level's first entity if this is the first entity in the level
    if(levelData[entityData[next.index].level].first == next)
		levelData[entityData[next.index].level].first = entity;
    // Update parent's firstChild if this is the first child
    if(GetEntityFirstChild(GetEntityParent(entity)) == entity)
		SetEntityFirstChild(GetEntityParent(entity), GetEntityNextSibling(entity));
    // Update last sibling's nextSibling if last sibling is valid
	SetEntityNextSibling(GetEntityLastSibling(entity), GetEntityNextSibling(entity));
    // Update next sibling's lastSibling if next sibling is valid
	SetEntityLastSibling(GetEntityNextSibling(entity), GetEntityLastSibling(entity));

    Entity parent = entityData[next.index].parent;
    Entity previous = entityData[next.index].lastSibling;

    // If next was the first child of its parent, entity will now be the first child instead
	if(entityData[parent.index].firstChild == next)
	{
		entityData[parent.index].firstChild = entity;
		onEntityLevelDataModified.Broadcast(parent);
	}
    // Set entity as the last sibling before next
	if(previous.IsValid())
	{
		entityData[previous.index].nextSibling = entity;
		onEntityLevelDataModified.Broadcast(previous);
	}
	entityData[next.index].lastSibling = entity;
	onEntityLevelDataModified.Broadcast(next);

    // Update entity's level data
    entityData[entity.index].level = entityData[next.index].level;
    entityData[entity.index].parent = parent;
    entityData[entity.index].nextSibling = next;
    entityData[entity.index].lastSibling = previous;
    entityData[entity.index].depth = entityData[next.index].depth;

	onEntityLevelDataModified.Broadcast(entity);

    return true;
}
bool EntityManager::MoveEntityAfter(Entity entity, Entity previous)
{
	assert((entity.IsValid() && previous.IsValid()) && "Invalid entities.");

    // If trying to parent the entity to itself or its children, ignore
    if(IsEntityChildOf(entity, previous))
        return false;

	onEntityMovedAfter.Broadcast(entity, previous);

    /* Update entity level data to set entity as the next sibling after previous */

    // Update parent's firstChild if this is the first child
    if(GetEntityFirstChild(GetEntityParent(entity)) == entity)
		SetEntityFirstChild(GetEntityParent(entity), GetEntityNextSibling(entity));
    // Update last sibling's nextSibling if last sibling is valid
	SetEntityNextSibling(GetEntityLastSibling(entity), GetEntityNextSibling(entity));
    // Update next sibling's lastSibling if next sibling is valid
	SetEntityLastSibling(GetEntityNextSibling(entity), GetEntityLastSibling(entity));

    Entity parent = entityData[previous.index].parent;
    Entity next = entityData[previous.index].nextSibling;

    // Set entity as the next sibling after previous
	if(next.IsValid())
	{
		entityData[next.index].lastSibling = entity;
		onEntityLevelDataModified.Broadcast(next);
	}
	entityData[previous.index].nextSibling = entity;
	onEntityLevelDataModified.Broadcast(previous);

    // Update entity's level data
    entityData[entity.index].level = entityData[previous.index].level;
    entityData[entity.index].parent = parent;
    entityData[entity.index].nextSibling = next;
    entityData[entity.index].lastSibling = previous;
    entityData[entity.index].depth = entityData[previous.index].depth;

	onEntityLevelDataModified.Broadcast(entity);

    return true;
}

void EntityManager::AddComponentOfType(Entity entity, std::type_index componentType, const void* component)
{
	assert(IsEntityValid(entity) && "Attempting to modify invalid entity.");

	assert(componentArrays.find(componentType) != componentArrays.end() && "Component has not been registered.");

	// First add all components that the given component depends on
	for(std::type_index dependencyID : GET_COMPONENT_DEPENDENCIES(componentType))
		if(!HasComponentOfType(entity, dependencyID))
			AddComponentOfType(entity, dependencyID);

	componentArrays[componentType]->Add(entity.index, component);
}
void EntityManager::RemoveComponentOfType(Entity entity, std::type_index componentType)
{
	assert(IsEntityValid(entity) && "Attempting to modify invalid entity.");

	assert(componentArrays.find(componentType) != componentArrays.end() && "Component has not been registered.");

	componentArrays[componentType]->Remove(entity.index);

	// After removing the component, also remove all components that depend on it
	for(std::type_index dependent : GET_COMPONENT_DEPENDENTS(componentType))
		if(HasComponentOfType(entity, dependent))
			RemoveComponentOfType(entity, dependent);
}
bool EntityManager::HasComponentOfType(Entity entity, std::type_index componentType)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	assert(componentArrays.find(componentType) != componentArrays.end() && "Component has not been registered.");

	return componentArrays[componentType]->Has(entity.index);
}
void* EntityManager::GetComponentOfType(Entity entity, std::type_index componentType)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	assert(componentArrays.find(componentType) != componentArrays.end() && "Component has not been registered.");

	return componentArrays[componentType]->GetRaw(entity.index);
}

void EntityManager::ForEachEntity(std::function<void(const Entity&)> callback) const
{
	for(const std::pair<LevelID, LevelData>& level : levelData)
		ForEachEntity(callback, level.first);
}
void EntityManager::ForEachEntity(std::function<void(const Entity&)> callback, LevelID level) const
{
	/* Depth-first recursive traversal; check for first child then next sibling */

	Entity child = levelData.at(level).first;
	while(child.IsValid())
	{
		ForEachEntity(callback, child);

		// Next child
		child = GetEntityNextSibling(child);
	}
}
void EntityManager::ForEachEntity(std::function<void(const Entity&)> callback, Entity root) const
{
	if(!IsEntityValid(root))
		return;

	callback(root);

	/* Depth-first recursive traversal; check for first child then next sibling */

	Entity child = GetEntityFirstChild(root);
	while(child.IsValid())
	{
		ForEachEntity(callback, child);

		// Next child
		child = GetEntityNextSibling(child);
	}
}
void EntityManager::ForEachEntity_Reversed(std::function<void(const Entity&)> callback) const
{
	std::vector<Entity> allEntities;
	// Gather all entities in order of normal iteration
	EntityManager::ForEachEntity(
		[&allEntities](const Entity& entity)
		{
			allEntities.push_back(entity);
		});

	// Process in reverse order
	for (int i = allEntities.size() - 1; i >= 0; i--)
	{
		const Entity& entity = allEntities[i];

		if (IsEntityValid(entity))
			callback(entity);
	}
}
void EntityManager::ForEachEntity_Reversed(std::function<void(const Entity&)> callback, LevelID level) const
{
	std::vector<Entity> allEntities;
	// Gather all entities in order of normal iteration
	EntityManager::ForEachEntity(
		[&allEntities](const Entity& entity)
		{
			allEntities.push_back(entity);
		}, level);

	// Process in reverse order
	for(int i = allEntities.size() - 1; i >= 0; i--)
	{
		const Entity& entity = allEntities[i];

		if(IsEntityValid(entity))
			callback(entity);
	}
}
void EntityManager::ForEachEntity_Reversed(std::function<void(const Entity&)> callback, Entity root) const
{
	std::vector<Entity> allEntities;
	// Gather all entities in order of normal iteration
	EntityManager::ForEachEntity(
		[&allEntities](const Entity& entity)
		{
			allEntities.push_back(entity);
		}, root);

	// Process in reverse order
	for(int i = allEntities.size() - 1; i >= 0; i--)
	{
		const Entity& entity = allEntities[i];

		if(IsEntityValid(entity))
			callback(entity);
	}
}

std::unordered_map<void*, std::type_index> EntityManager::GetAllComponents(Entity entity)
{
	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	std::unordered_map<void*, std::type_index> components;
	
	for(auto& componentArray : componentArrays)
	{
		if(componentArray.second->Has(entity.index))
		{
			void* componentPtr = componentArray.second->GetRaw(entity.index);

			components.emplace(componentPtr, componentArray.first);
		}
	}

	return components;
}

bool EntityManager::IsEntityValid(Entity entity) const
{
	// Entity index must be valid and of the same generation
	return entity.index < nextEntity && entityData[entity.index].valid && entityData[entity.index].generationCount == entity.generation;
}
bool EntityManager::IsComponentValid(Entity entity, std::type_index componentType) const
{
	if(!EntityManager::IsEntityValid(entity))
		return false;

	assert(IsEntityValid(entity) && "Attempting to access invalid entity.");

	// Component is invalid if it isn't registered
	if(componentArrays.find(componentType) == componentArrays.end())
		return false;

	return componentArrays.at(componentType)->Has(entity.index);
}

bool EntityManager::IsEntityChildOf(Entity parent, Entity child) const
{
	bool isChild = false;
    // Look for the entity in parent's children
    ForEachEntity([child, &isChild](const Entity& entity)
        {
            if(entity == child)
            {
                isChild = true;
                return;
            }
        }, parent);

    return isChild;
}

std::string EntityManager::GetEntityName(Entity entity) const
{
	return (IsEntityValid(entity) ? entityData[entity.index] : EntityData()).name;
}
LevelID EntityManager::GetEntityLevel(Entity entity) const
{
	return (IsEntityValid(entity) ? entityData[entity.index] : EntityData()).level;
}
std::filesystem::path EntityManager::GetEntityPrefabPath(Entity entity) const
{
	return (IsEntityValid(entity) ? entityData[entity.index] : EntityData()).prefabPath;
}
Entity EntityManager::GetEntityParent(Entity entity) const
{
	return (IsEntityValid(entity) ? entityData[entity.index] : EntityData()).parent;
}
Entity EntityManager::GetEntityFirstChild(Entity entity) const
{
	return (IsEntityValid(entity) ? entityData[entity.index] : EntityData()).firstChild;
}
Entity EntityManager::GetEntityNextSibling(Entity entity) const
{
	return (IsEntityValid(entity) ? entityData[entity.index] : EntityData()).nextSibling;
}
Entity EntityManager::GetEntityLastSibling(Entity entity) const
{
	return (IsEntityValid(entity) ? entityData[entity.index] : EntityData()).lastSibling;
}
uint32_t EntityManager::GetEntityDepth(Entity entity) const
{
	return (IsEntityValid(entity) ? entityData[entity.index] : EntityData()).depth;
}

void EntityManager::SetEntityName(const Entity& entity, const std::string& name)
{
	assert(entity.IsValid() && "Invalid entity.");

	entityData[entity.index].name = name;

	onEntityLevelDataModified.Broadcast(entity);
}

void EntityManager::SaveEntityAsPrefab(const Entity& entity, const std::filesystem::path& prefabPath)
{
	assert(IsEntityValid(entity) && "Invalid entity.");
	
	// Associate this entity with the prefab
	entityData[entity.index].prefabPath = prefabPath;
	onEntityLevelDataModified.Broadcast(entity);

	std::string prefabName = GetEntityName(entity);

	/* Save the entity hierarchy (all children will be saved recursively) */

	SerializedObject prefabObject = SaveEntity(entity);

	/* Save the object to file */

	std::ofstream fileOut(GetGameAssetsPath().append(prefabPath.string()));
	File::SaveObject(fileOut, prefabObject);
	fileOut.close();

	LOG((*world), "Prefab Saved: {}", prefabName);
}
Entity EntityManager::LoadEntityFromPrefab(const std::filesystem::path& prefabPath, const Entity& parent)
{
	/* Load the object from file */

	std::ifstream fileIn(GetGameAssetsPath().append(prefabPath.string()));
	SerializedObject prefabObject = File::LoadObject(fileIn);
	fileIn.close();

	// The prefab itself is an entity, so we can just load it normally (all children will be loaded recursively)
	Entity newEntity = LoadEntity(prefabObject, parent);
	
	LOG((*world), "Level Loaded: {}", prefabObject.parameters["Name"]);

	return newEntity;
}

void EntityManager::SaveLevel(const std::filesystem::path& levelPath)
{
	std::string levelName = levelPath.filename().replace_extension().string();

	SerializedObject levelObject = {};
	levelObject.typeName = "Level";
	levelObject.parameters.emplace("Name", levelName);

	/* Save all top-level entities in the level (each entity will recursively save their children) */

	Entity entity = GetEntityFirstChild({});
	while(entity.IsValid())
	{
		levelObject.children.push_back(SaveEntity(entity));

		entity = GetEntityNextSibling(entity);
	}

	/* Save the object to file */

	std::ofstream fileOut(GetGameAssetsPath().append(levelPath.string()));
	File::SaveObject(fileOut, levelObject);
	fileOut.close();

	LOG((*world), "Level Saved: {}", levelName);
}
void EntityManager::LoadLevel(const std::filesystem::path& levelPath)
{
	/* Load the object from file */

	std::cout << GetGameAssetsPath().append(levelPath.string()).string() << std::endl;
	std::ifstream fileIn(GetGameAssetsPath().append(levelPath.string()));
	SerializedObject levelObject = File::LoadObject(fileIn);
	fileIn.close();

	// Load each top-level entity as a child of the root (each entity will recursively load its children)
	for(SerializedObject& child : levelObject.children)
		LoadEntity(child, {});
	
	LOG((*world), "Level Loaded: {}", levelObject.parameters["Name"]);
}

SerializedObject EntityManager::SaveEntity(Entity entity)
{
	SerializedObject entityObject = {};
	entityObject.typeName = "Entity";
	entityObject.parameters.emplace("Name", GetEntityName(entity));
	entityObject.parameters.emplace("PrefabPath", GetEntityPrefabPath(entity).string());

	std::unordered_map<void*, std::type_index> components = EntityManager::GetAllComponents(entity);
	for(auto& component : components)
	{
		SerializedObject componentObject;
		componentObject.typeName = "Component";
		componentObject.parameters.emplace("TypeName", GET_COMPONENT_NAME(component.second));

		std::vector<std::pair<void*, ComponentVariableInfo>> variables = GET_COMPONENT_VARS(component.second, component.first);
		for(std::pair<void*, ComponentVariableInfo>& variable : variables)
		{
			/* Use a string stream to write the value to a string */

			std::stringstream ss;
			variable.second.writeToFile(ss, variable.first);
			
			std::string valueString;
			std::getline(ss, valueString);

			componentObject.parameters.emplace(variable.second.variableName, valueString);
		}

		entityObject.children.push_back(componentObject);
	}

	/* Add all child entities as children of the serialized object */
	Entity child = GetEntityFirstChild(entity);
	while(child.IsValid())
	{
		entityObject.children.push_back(SaveEntity(child));

		child = GetEntityNextSibling(child);
	}

	return entityObject;
}
Entity EntityManager::LoadEntity(const SerializedObject& entityObject, Entity parent)
{
	assert(entityObject.typeName == "Entity");

	Entity newEntity = CreateEntity(parent, entityObject.parameters.at("Name"), entityObject.parameters.at("PrefabPath"));

	for(const SerializedObject& child : entityObject.children)
	{
		if(child.typeName == "Component")
		{
			uint32_t componentTypeIndex = 0xFFFFFFFF;

			// Find the component type with the given name
			for(uint32_t j = 0; j < GetRegisteredComponentsList().size(); j++)
				if(GET_COMPONENT_NAME(GetRegisteredComponentsList()[j]) == child.parameters.at("TypeName"))
				{
					componentTypeIndex = j;
					break;
				}

			assert(componentTypeIndex < 0xFFFFFFFF && "Unregistered component read in from file!");

			std::type_index componentType = GetRegisteredComponentsList()[componentTypeIndex];

			AddComponentOfType(newEntity, componentType);
			void* newComponent = GetComponentOfType(newEntity, componentType);
			std::vector<std::pair<void*, ComponentVariableInfo>> variables = GET_COMPONENT_VARS(componentType, newComponent);
			// Load variable values as string streams and parse them by type
			// Variables are searched for by name
			for(auto& variable : variables)
			{
				std::stringstream ss(child.parameters.at(variable.second.variableName));
				variable.second.readFromFile(ss, variable.first);
			}
		}
		// Load child entities recursively
		if(child.typeName == "Entity")
			LoadEntity(child, newEntity);
	}

	return newEntity;
}

EntityData& EntityManager::GetEntityData(const Entity& entity)
{
	assert(IsEntityValid(entity) && "Entity is invalid.");
	return entityData[entity.index];
}

Entity EntityManager::GetEntityByIndex(uint32_t index)
{
	Entity entity = {};
	entity.index = index;
	entity.generation = index == INVALID_ENTITY ? INVALID_ENTITY : entityData[index].generationCount;

	return entity;
}

void EntityManager::SetEntityParent(Entity entity, Entity parent)
{
	if(IsEntityValid(entity))
		entityData[entity.index].parent = parent;
}
void EntityManager::SetEntityFirstChild(Entity entity, Entity firstChild)
{
	if(IsEntityValid(entity))
		entityData[entity.index].firstChild = firstChild;
}
void EntityManager::SetEntityNextSibling(Entity entity, Entity nextSibling)
{
	if(IsEntityValid(entity))
		entityData[entity.index].nextSibling = nextSibling;
}
void EntityManager::SetEntityLastSibling(Entity entity, Entity lastSibling)
{
	if(IsEntityValid(entity))
		entityData[entity.index].lastSibling = lastSibling;
}