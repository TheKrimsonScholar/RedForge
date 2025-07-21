#pragma once

#include <fstream>

#include "EntityManager.h"

#include "Exports.h"

struct EntityLevelData
{
	uint32_t levelIndex;
	std::string name = "";
	uint32_t parentIndex = INVALID_ENTITY;
};

class LevelManager
{
private:
	static inline LevelManager* Instance;

	std::vector<Entity> entities;
	std::unordered_map<uint32_t, EntityLevelData> entityLevelDataMap;

public:
	LevelManager() {};
	~LevelManager() {};

	void Startup();
	void Shutdown();

	REDFORGE_API static Entity CreateEntity(std::string name = "", Entity parent = {});
	REDFORGE_API static void DestroyEntity(Entity entity);

	REDFORGE_API static uint32_t GetLevelIndex(Entity entity);
	REDFORGE_API static std::string GetName(Entity entity);
	REDFORGE_API static Entity GetParent(Entity entity);

	REDFORGE_API static Entity GetEntity(uint32_t levelIndex);

	REDFORGE_API static std::vector<Entity> GetAllEntities() { return Instance->entities; };

	REDFORGE_API static void SaveLevel();
	REDFORGE_API static void LoadLevel();

private:
	void SaveEntity(std::ofstream outFile, Entity entity);
};