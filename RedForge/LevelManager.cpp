#include "LevelManager.h"

#include <sstream>

#include "EntityManager.h"

#include "DebugMacros.h"

void LevelManager::Startup()
{
	Instance = this;
}
void LevelManager::Shutdown()
{

}

Entity LevelManager::CreateEntity(std::string name, Entity parent, const std::filesystem::path& prefabPath)
{
	// Total number of entites created; used for default entity names
	static uint32_t createdCount = 0;

	createdCount++;

	//assert(EntityManager::IsEntityValid(parent) && "Parent entity is invalid."); Invalid parent will instead be interpreted as no parent

	/* Insert the new entity as parent's last child */

	//uint32_t newEntityLevelIndex = Instance->entities.size(); // If no parent, insert at the end of the entities list
	
	//assert(Instance->entityLevelDataMap.find(parent) != Instance->entityLevelDataMap.end() && "Parent entity not in level.");
	
	Entity lastSibling = GetEntityFirstChild(parent);
	// Find the last child of the parent
	while(GetEntityNextSibling(lastSibling).IsValid())
		lastSibling = GetEntityNextSibling(lastSibling);
	
	Entity newEntity = EntityManager::CreateEntity();

	EntityLevelData entityLevelData = {};
	//entityLevelData.levelIndex = newEntityLevelIndex;
	entityLevelData.name = name == "" ? "Entity " + std::to_string(createdCount) : name;
	entityLevelData.prefabPath = prefabPath;
	entityLevelData.parent = parent;
	entityLevelData.firstChild = {};
	entityLevelData.nextSibling = {};
	entityLevelData.lastSibling = lastSibling;
	entityLevelData.depth = GetEntityDepth(parent) + 1;

	// If parent doesn't have any children yet, this is the first child
	if(!Instance->entityLevelDataMap[parent].firstChild.IsValid())
		Instance->entityLevelDataMap[parent].firstChild = newEntity;
	// If there is a sibling before this entity, update it
	if(lastSibling.IsValid())
		Instance->entityLevelDataMap[lastSibling].nextSibling = newEntity;

	//Instance->entities.insert(Instance->entities.begin() + newEntityLevelIndex, newEntity);
	Instance->entityLevelDataMap.emplace(newEntity, entityLevelData);

	Instance->onEntityCreated.Broadcast(newEntity);

	return newEntity;
}
void LevelManager::DestroyEntity(Entity entity)
{
	// Silently return if entity is invalid or not present in level
	if(!EntityManager::IsEntityValid(entity) || Instance->entityLevelDataMap.find(entity) == Instance->entityLevelDataMap.end())
		return;

	Instance->onEntityDestroyed.Broadcast(entity);

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
	for(Entity& child : children)
	{
		// Remove child from level - don't need to update linking between nodes because all linked nodes will also be removed
		Instance->entityLevelDataMap.erase(child);

		// Destroy child
		EntityManager::DestroyEntity(child);
	}

	//uint32_t entityLevelIndex = GetLevelIndex(entity);

	// Shift down each entity that comes after the one being destroyed
	//for(uint32_t i = entityLevelIndex + 1; i < Instance->entities.size(); i++)
		//Instance->entityLevelDataMap[Instance->entities[i].index].levelIndex--;

	Entity parent = GetEntityParent(entity);
	Entity lastSibling = GetEntityLastSibling(entity);
	Entity nextSibling = GetEntityNextSibling(entity);
	
	// If this entity is the first child, set its next sibling as first child
	if(Instance->entityLevelDataMap[parent].firstChild == entity)
		Instance->entityLevelDataMap[parent].firstChild = nextSibling;
	// If the last sibling exists, set its next sibling to the sibling after this entity
	if(lastSibling.IsValid())
		Instance->entityLevelDataMap[lastSibling].nextSibling = Instance->entityLevelDataMap[entity].nextSibling;
	// If the next sibling exists, set its last sibling to the sibling before this entity
	if(nextSibling.IsValid())
		Instance->entityLevelDataMap[nextSibling].lastSibling = Instance->entityLevelDataMap[entity].lastSibling;

	// Remove entity from level
	//Instance->entities.erase(Instance->entities.begin() + entityLevelIndex);
	Instance->entityLevelDataMap.erase(entity);

	// Destroy entity
	EntityManager::DestroyEntity(entity);
}

bool LevelManager::SetEntityParent(Entity entity, Entity newParent)
{
	assert(entity.IsValid() && "Invalid entity.");

    // If trying to parent the entity to itself or its children, ignore
    if(IsEntityChildOf(entity, newParent))
        return false;

	Instance->onEntityReparented.Broadcast(entity, newParent);

    /* Update entity level data to set entity as the last child of newParent */

    // Update parent's firstChild if this is the first child
    if(Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].parent].firstChild == entity)
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].parent].firstChild = Instance->entityLevelDataMap[entity].nextSibling;
    // Update last sibling's nextSibling if last sibling is valid
    if(Instance->entityLevelDataMap[entity].lastSibling.IsValid())
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].lastSibling].nextSibling = Instance->entityLevelDataMap[entity].nextSibling;
    // Update next sibling's lastSibling if next sibling is valid
    if(Instance->entityLevelDataMap[entity].nextSibling.IsValid())
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].nextSibling].lastSibling = Instance->entityLevelDataMap[entity].lastSibling;

    Entity lastSibling = Instance->entityLevelDataMap[newParent].firstChild;
    // Find last child of newParent
    while(Instance->entityLevelDataMap[lastSibling].nextSibling.IsValid())
        lastSibling = Instance->entityLevelDataMap[lastSibling].nextSibling;

    // If lastSibling is valid, set entity as the next child after it
	if(lastSibling.IsValid())
	{
		Instance->entityLevelDataMap[lastSibling].nextSibling = entity;
		Instance->onEntityLevelDataModified.Broadcast(lastSibling);
	}
    // Otherwise newParent has no children, so set entity as firstChild instead
    else
		Instance->entityLevelDataMap[newParent].firstChild = entity;

    // Update entity's level data
    Instance->entityLevelDataMap[entity].parent = newParent;
    Instance->entityLevelDataMap[entity].nextSibling = {};
    Instance->entityLevelDataMap[entity].lastSibling = lastSibling;
    Instance->entityLevelDataMap[entity].depth = Instance->entityLevelDataMap[newParent].depth + 1;

	Instance->onEntityLevelDataModified.Broadcast(entity);

    return true;
}
bool LevelManager::MoveEntityBefore(Entity entity, Entity next)
{
	assert((entity.IsValid() && next.IsValid()) && "Invalid entities.");

    // If trying to move the entity to be a sibling of itself or one of its children, ignore
    if(IsEntityChildOf(entity, next))
        return false;

	Instance->onEntityMovedBefore.Broadcast(entity, next);

    /* Update entity level data to set entity as the last sibling before next */

    // Update parent's firstChild if this is the first child
    if(Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].parent].firstChild == entity)
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].parent].firstChild = Instance->entityLevelDataMap[entity].nextSibling;
    // Update last sibling's nextSibling if last sibling is valid
    if(Instance->entityLevelDataMap[entity].lastSibling.IsValid())
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].lastSibling].nextSibling = Instance->entityLevelDataMap[entity].nextSibling;
    // Update next sibling's lastSibling if next sibling is valid
    if(Instance->entityLevelDataMap[entity].nextSibling.IsValid())
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].nextSibling].lastSibling = Instance->entityLevelDataMap[entity].lastSibling;

    Entity parent = Instance->entityLevelDataMap[next].parent;
    Entity previous = Instance->entityLevelDataMap[next].lastSibling;

    // If next was the first child of its parent, entity will now be the first child instead
	if(Instance->entityLevelDataMap[parent].firstChild == next)
	{
		Instance->entityLevelDataMap[parent].firstChild = entity;
		Instance->onEntityLevelDataModified.Broadcast(parent);
	}
    // Set entity as the last sibling before next
	if(previous.IsValid())
	{
		Instance->entityLevelDataMap[previous].nextSibling = entity;
		Instance->onEntityLevelDataModified.Broadcast(previous);
	}
	Instance->entityLevelDataMap[next].lastSibling = entity;
	Instance->onEntityLevelDataModified.Broadcast(next);

    // Update entity's level data
    Instance->entityLevelDataMap[entity].parent = parent;
    Instance->entityLevelDataMap[entity].nextSibling = next;
    Instance->entityLevelDataMap[entity].lastSibling = previous;
    Instance->entityLevelDataMap[entity].depth = Instance->entityLevelDataMap[next].depth;

	Instance->onEntityLevelDataModified.Broadcast(entity);

    return true;
}
bool LevelManager::MoveEntityAfter(Entity entity, Entity previous)
{
	assert((entity.IsValid() && previous.IsValid()) && "Invalid entities.");

    // If trying to parent the entity to itself or its children, ignore
    if(IsEntityChildOf(entity, previous))
        return false;

	Instance->onEntityMovedAfter.Broadcast(entity, previous);

    /* Update entity level data to set entity as the next sibling after previous */

    // Update parent's firstChild if this is the first child
    if(Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].parent].firstChild == entity)
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].parent].firstChild = Instance->entityLevelDataMap[entity].nextSibling;
    // Update last sibling's nextSibling if last sibling is valid
    if(Instance->entityLevelDataMap[entity].lastSibling.IsValid())
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].lastSibling].nextSibling = Instance->entityLevelDataMap[entity].nextSibling;
    // Update next sibling's lastSibling if next sibling is valid
    if(Instance->entityLevelDataMap[entity].nextSibling.IsValid())
		Instance->entityLevelDataMap[Instance->entityLevelDataMap[entity].nextSibling].lastSibling = Instance->entityLevelDataMap[entity].lastSibling;

    Entity parent = Instance->entityLevelDataMap[previous].parent;
    Entity next = Instance->entityLevelDataMap[previous].nextSibling;

    // Set entity as the next sibling after previous
	if(next.IsValid())
	{
		Instance->entityLevelDataMap[next].lastSibling = entity;
		Instance->onEntityLevelDataModified.Broadcast(next);
	}
	Instance->entityLevelDataMap[previous].nextSibling = entity;
	Instance->onEntityLevelDataModified.Broadcast(previous);

    // Update entity's level data
    Instance->entityLevelDataMap[entity].parent = parent;
    Instance->entityLevelDataMap[entity].nextSibling = next;
    Instance->entityLevelDataMap[entity].lastSibling = previous;
    Instance->entityLevelDataMap[entity].depth = Instance->entityLevelDataMap[previous].depth;

	Instance->onEntityLevelDataModified.Broadcast(entity);

    return true;
}

void LevelManager::ForEachEntity(std::function<void(const Entity&)> callback, Entity root)
{
	if(EntityManager::IsEntityValid(root))
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
void LevelManager::ForEachEntity_Reversed(std::function<void(const Entity&)> callback, Entity root)
{
	std::vector<Entity> allEntities;
	// Gather all entities in order of normal iteration
	LevelManager::ForEachEntity(
		[&allEntities](const Entity& entity)
		{
			allEntities.push_back(entity);
		}, root);

	// Process in reverse order
	for(int i = allEntities.size() - 1; i >= 0; i--)
	{
		const Entity& entity = allEntities[i];

		if(EntityManager::IsEntityValid(entity))
			callback(entity);
	}
}

void LevelManager::SaveEntityAsPrefab(const Entity& entity, const std::filesystem::path& prefabPath)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	
	// Associate this entity with the prefab
	Instance->entityLevelDataMap[entity].prefabPath = prefabPath;
	Instance->onEntityLevelDataModified.Broadcast(entity);

	std::string prefabName = GetEntityName(entity);

	/* Save the entity hierarchy (all children will be saved recursively) */

	SerializedObject prefabObject = SaveEntity(entity);

	/* Save the object to file */

	std::ofstream fileOut(GetGameAssetsPath().append(prefabPath.string()));
	FileManager::SaveObject(fileOut, prefabObject);
	fileOut.close();

	LOG("Prefab Saved: {}", prefabName);
}
Entity LevelManager::LoadEntityFromPrefab(const std::filesystem::path& prefabPath, const Entity& parent)
{
	/* Load the object from file */

	std::ifstream fileIn(GetGameAssetsPath().append(prefabPath.string()));
	SerializedObject prefabObject = FileManager::LoadObject(fileIn);
	fileIn.close();

	// The prefab itself is an entity, so we can just load it normally (all children will be loaded recursively)
	Entity newEntity = LoadEntity(prefabObject, parent);
	
	LOG("Level Loaded: {}", prefabObject.parameters["Name"]);

	return newEntity;
}

void LevelManager::SaveLevel(const std::filesystem::path& levelPath)
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
	FileManager::SaveObject(fileOut, levelObject);
	fileOut.close();

	LOG("Level Saved: {}", levelName);
}
void LevelManager::LoadLevel(const std::filesystem::path& levelPath)
{
	/* Load the object from file */

	std::cout << GetGameAssetsPath().append(levelPath.string()).string() << std::endl;
	std::ifstream fileIn(GetGameAssetsPath().append(levelPath.string()));
	SerializedObject levelObject = FileManager::LoadObject(fileIn);
	fileIn.close();

	// Load each top-level entity as a child of the root (each entity will recursively load its children)
	for(SerializedObject& child : levelObject.children)
		LoadEntity(child, {});
	
	LOG("Level Loaded: {}", levelObject.parameters["Name"]);
}

SerializedObject LevelManager::SaveEntity(Entity entity)
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
Entity LevelManager::LoadEntity(const SerializedObject& entityObject, Entity parent)
{
	assert(entityObject.typeName == "Entity");

	Entity newEntity = CreateEntity(entityObject.parameters.at("Name"), parent, entityObject.parameters.at("PrefabPath"));

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

			void* newComponent = EntityManager::AddComponentOfType(newEntity, componentType);
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

std::vector<Entity> LevelManager::GetAllEntityAncestors(const Entity& entity)
{
	Entity currentAncestor = GetEntityParent(entity);
	std::vector<Entity> ancestors;
	while(currentAncestor.IsValid())
	{
		ancestors.push_back(currentAncestor);
		currentAncestor = GetEntityParent(currentAncestor);
	}
	return ancestors;
}
bool LevelManager::IsEntityAncestorContainedInList(const Entity& entity, const std::vector<Entity>& list)
{
	std::vector<Entity> ancestors = GetAllEntityAncestors(entity);
	std::unordered_set<Entity> ancestorSet(ancestors.begin(), ancestors.end());

	// Check for overlap between ancestor set and given entity list
	for(const Entity& e : list)
		if(ancestorSet.contains(e))
			return true;

	return false;
}

bool LevelManager::IsEntityChildOf(Entity parent, Entity child)
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

std::string LevelManager::GetEntityName(Entity entity)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].name;
}
std::filesystem::path LevelManager::GetEntityPrefabPath(Entity entity)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].prefabPath;
}
Entity LevelManager::GetEntityParent(Entity entity)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].parent;
}
Entity LevelManager::GetEntityFirstChild(Entity entity)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].firstChild;
}
Entity LevelManager::GetEntityNextSibling(Entity entity)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].nextSibling;
}
Entity LevelManager::GetEntityLastSibling(Entity entity)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].lastSibling;
}
uint32_t LevelManager::GetEntityDepth(Entity entity)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].depth;
}

void LevelManager::SetEntityName(const Entity& entity, const std::string& name)
{
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	Instance->entityLevelDataMap[entity].name = name;

	Instance->onEntityLevelDataModified.Broadcast(entity);
}