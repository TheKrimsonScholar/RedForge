#include "LevelManager.h"

#include "EntityManager.h"

void LevelManager::Startup()
{
	Instance = this;
}
void LevelManager::Shutdown()
{

}

void LevelManager::SaveLevel()
{
	std::ofstream file("level.txt");

	std::vector<Entity> entities = GetAllEntities();

	file << entities.size() << "\n";

	for(Entity& entity : entities)
	{
		assert(EntityManager::IsEntityValid(entity) && "Invalid entity in level!");

		file << GetName(entity) << "\n";
		file << GetLevelIndex(GetParent(entity)) << "\n";

		std::unordered_map<void*, std::type_index> components = EntityManager::GetAllComponents(entity);
		file << components.size() << "\n";
		for(auto& component : components)
		{
			file << GET_COMPONENT_NAME(component.second) << "\n";

			std::vector<std::pair<void*, ComponentVariableInfo>> variables = GET_COMPONENT_VARS(component.second, component.first);
			for(std::pair<void*, ComponentVariableInfo>& variable : variables)
			{
				//file << variable.second.variableType.name() << "\n";

				variable.second.writeToFile(file, variable.first);
			}
		}
	}

	file.close();
}
void LevelManager::LoadLevel()
{
	std::ifstream file("level.txt");

	uint32_t entityCount;
	file >> entityCount;

	// Skip line break
	file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// Read in all entities
	for(uint32_t i = 0; i < entityCount; i++)
	{
		std::string name;
		std::getline(file, name);

		Entity newEntity = CreateEntity();

		uint32_t parent;
		file >> parent;

		uint32_t componentsCount;
		file >> componentsCount;

		// Read in all components
		for(uint32_t i = 0; i < componentsCount; i++)
		{
			std::string componentName;
			file >> componentName;

			uint32_t componentTypeIndex = 0xFFFFFFFF;

			// Find the component type with the given name
			for(uint32_t j = 0; j < GetRegisteredComponentsList().size(); j++)
				if(GET_COMPONENT_NAME(GetRegisteredComponentsList()[j]) == componentName)
				{
					componentTypeIndex = j;
					break;
				}

			assert(componentTypeIndex < 0xFFFFFFFF && "Unregistered component read in from file!");

			std::type_index componentType = GetRegisteredComponentsList()[componentTypeIndex];

			void* newComponent = EntityManager::AddComponentOfType(newEntity, componentType);
			std::vector<std::pair<void*, ComponentVariableInfo>> variables = GET_COMPONENT_VARS(componentType, newComponent);
			for(auto& variable : variables)
				variable.second.readFromFile(file, variable.first);
		}
	}

	file.close();
}

Entity LevelManager::CreateEntity(std::string name, Entity parent)
{
	// Total number of entites created; used for default entity names
	static uint32_t createdCount = 0;

	createdCount++;

	//assert(EntityManager::IsEntityValid(parent) && "Parent entity is invalid."); Invalid parent will instead be interpreted as no parent

	/* Determine the level index of the new entity (insert it as the parent's last child) */

	uint32_t newEntityLevelIndex = Instance->entities.size(); // If no parent, insert at the end of the entities list
	if(EntityManager::IsEntityValid(parent))
	{
		assert(Instance->entityLevelDataMap.find(parent.index) != Instance->entityLevelDataMap.end() && "Parent entity not in level.");
		
		// Find the last child index of the parent
		newEntityLevelIndex = GetLevelIndex(parent) + 1;
		/*while(GetParent(Instance->entities[newEntityLevelIndex]) == parent)
			newEntityLevelIndex++;*/
	}

	// Shift up each entity that comes after the insertion point
	for(uint32_t i = newEntityLevelIndex; i < Instance->entities.size(); i++)
		Instance->entityLevelDataMap[Instance->entities[i].index].levelIndex++;
	
	Entity newEntity = EntityManager::CreateEntity();

	EntityLevelData sceneData = {};
	sceneData.levelIndex = newEntityLevelIndex;
	sceneData.name = name == "" ? "Entity " + std::to_string(createdCount) : name;
	sceneData.parentIndex = parent.index;

	//Instance->entities.push_back(newEntity);
	Instance->entities.insert(Instance->entities.begin() + newEntityLevelIndex, newEntity);
	Instance->entityLevelDataMap.emplace(newEntity.index, sceneData);

	return newEntity;
}
void LevelManager::DestroyEntity(Entity entity)
{
	assert(EntityManager::IsEntityValid(entity) && "Attempting to destroy invalid entity.");

	uint32_t entityLevelIndex = GetLevelIndex(entity);

	// Shift down each entity that comes after the one being destroyed
	for(uint32_t i = entityLevelIndex + 1; i < Instance->entities.size(); i++)
		Instance->entityLevelDataMap[Instance->entities[i].index].levelIndex--;

	// Remove entity from level
	Instance->entities.erase(Instance->entities.begin() + entityLevelIndex);
	Instance->entityLevelDataMap.erase(entity.index);

	// Destroy entity
	EntityManager::DestroyEntity(entity);
}

uint32_t LevelManager::GetLevelIndex(Entity entity)
{
	// If valid and in level, get from data map
	if(EntityManager::IsEntityValid(entity) && Instance->entityLevelDataMap.find(entity.index) != Instance->entityLevelDataMap.end())
		return Instance->entityLevelDataMap[entity.index].levelIndex;
	// Otherwise return invalid
	else
		return INVALID_ENTITY;
}
std::string LevelManager::GetName(Entity entity)
{
	assert(EntityManager::IsEntityValid(entity) && "Trying to access invalid entity.");

	assert(Instance->entityLevelDataMap.find(entity.index) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return Instance->entityLevelDataMap[entity.index].name;
}
Entity LevelManager::GetParent(Entity entity)
{
	assert(EntityManager::IsEntityValid(entity) && "Trying to access invalid entity.");

	assert(Instance->entityLevelDataMap.find(entity.index) != Instance->entityLevelDataMap.end() && "Entity not in level.");
	return EntityManager::GetEntityByIndex(Instance->entityLevelDataMap[entity.index].parentIndex);
}

void LevelManager::SaveEntity(std::ofstream outFile, Entity entity)
{
	std::unordered_map<void*, std::type_index> components = EntityManager::GetAllComponents(entity);
	for(auto& componentPtrType : components)
	{
		GET_COMPONENT_VARS(componentPtrType.second, componentPtrType.first);
	}
}