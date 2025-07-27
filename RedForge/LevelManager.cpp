#include "LevelManager.h"

#include <sstream>

#include "EntityManager.h"

void LevelManager::Startup()
{
	Instance = this;
}
void LevelManager::Shutdown()
{

}

Entity LevelManager::CreateEntity(std::string name, Entity parent)
{
	// Total number of entites created; used for default entity names
	static uint32_t createdCount = 0;

	createdCount++;

	//assert(EntityManager::IsEntityValid(parent) && "Parent entity is invalid."); Invalid parent will instead be interpreted as no parent

	/* Determine the level index of the new entity (insert it as the parent's last child) */

	//uint32_t newEntityLevelIndex = Instance->entities.size(); // If no parent, insert at the end of the entities list
	Entity lastSibling = {};
	if(EntityManager::IsEntityValid(parent))
	{
		assert(Instance->entityLevelDataMap.find(parent) != Instance->entityLevelDataMap.end() && "Parent entity not in level.");
		
		// Find the last child of the parent
		lastSibling = GetEntityFirstChild(parent);
		while(GetEntityNextSibling(lastSibling).IsValid())
			lastSibling = GetEntityNextSibling(lastSibling);
	}
	else
		parent = {};

	// Shift up each entity that comes after the insertion point
	//for(uint32_t i = newEntityLevelIndex; i < Instance->entities.size(); i++)
		//Instance->entityLevelDataMap[Instance->entities[i].index].levelIndex++;
	
	Entity newEntity = EntityManager::CreateEntity();

	EntityLevelData entityLevelData = {};
	//entityLevelData.levelIndex = newEntityLevelIndex;
	entityLevelData.name = name == "" ? "Entity " + std::to_string(createdCount) : name;
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

	return newEntity;
}
void LevelManager::DestroyEntity(Entity entity)
{
	assert(EntityManager::IsEntityValid(entity) && "Attempting to destroy invalid entity.");
	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Attempting to destroy entity not in level.");

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

//uint32_t LevelManager::GetLevelIndex(Entity entity)
//{
//	// If valid and in level, get from data map
//	if(EntityManager::IsEntityValid(entity) && Instance->entityLevelDataMap.find(entity.index) != Instance->entityLevelDataMap.end())
//		return Instance->entityLevelDataMap[entity.index].levelIndex;
//	// Otherwise return invalid
//	else
//		return INVALID_ENTITY;
//}
std::string LevelManager::GetEntityName(Entity entity)
{
	//assert(EntityManager::IsEntityValid(entity) && "Trying to access invalid entity.");

	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].name;
}
Entity LevelManager::GetEntityParent(Entity entity)
{
	//assert(EntityManager::IsEntityValid(entity) && "Trying to access invalid entity.");

	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].parent;
}
Entity LevelManager::GetEntityFirstChild(Entity entity)
{
	//assert(EntityManager::IsEntityValid(entity) && "Trying to access invalid entity.");

	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].firstChild;
}
Entity LevelManager::GetEntityNextSibling(Entity entity)
{
	//assert(EntityManager::IsEntityValid(entity) && "Trying to access invalid entity.");

	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].nextSibling;
}
Entity LevelManager::GetEntityLastSibling(Entity entity)
{
	//assert(EntityManager::IsEntityValid(entity) && "Trying to access invalid entity.");

	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].lastSibling;
}
uint32_t LevelManager::GetEntityDepth(Entity entity)
{
	//assert(EntityManager::IsEntityValid(entity) && "Trying to access invalid entity.");

	assert(Instance->entityLevelDataMap.find(entity) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity].depth;
}

void LevelManager::ForEachEntity(std::function<void(const Entity&)> callback, Entity root)
{
	if(EntityManager::IsEntityValid(root))
		callback(root);

	/* Depth-first traversal; check for first child then next sibling */

	Entity firstChild = GetEntityFirstChild(root);
	Entity nextSibling = GetEntityNextSibling(root);
	if(firstChild.IsValid())
		ForEachEntity(callback, firstChild);
	if(nextSibling.IsValid())
		ForEachEntity(callback, nextSibling);
}

void LevelManager::SaveLevel()
{
	SerializedObject levelObject = {};
	levelObject.typeName = "Level";
	levelObject.parameters.emplace("Name", "Level0");

	/* Save all top-level entities in the level (each entity will recursively save their children) */

	Entity entity = GetEntityFirstChild({});
	while(entity.IsValid())
	{
		levelObject.children.push_back(SaveEntity(entity));

		entity = GetEntityNextSibling(entity);
	}

	/* Save the object to file */

	std::ofstream fileOut("level.txt");
	FileManager::SaveObject(fileOut, levelObject);
	fileOut.close();
}
void LevelManager::LoadLevel()
{
	/* Load the object from file */

	std::ifstream fileIn("level.txt");
	SerializedObject levelObject = FileManager::LoadObject(fileIn);
	fileIn.close();

	// Load each top-level entity as a child of the root (each entity will recursively load its children)
	for(SerializedObject& child : levelObject.children)
		LoadEntity(child, {});
}

SerializedObject LevelManager::SaveEntity(Entity entity)
{
	SerializedObject entityObject = {};
	entityObject.typeName = "Entity";
	entityObject.parameters.emplace("Name", GetEntityName(entity));

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
void LevelManager::LoadEntity(const SerializedObject& entityObject, Entity parent)
{
	assert(entityObject.typeName == "Entity");

	Entity newEntity = CreateEntity(entityObject.parameters.at("Name"), parent);

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
}