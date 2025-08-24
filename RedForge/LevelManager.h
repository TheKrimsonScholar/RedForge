#pragma once

#include <fstream>

#include "Event.h"
#include "EntityManager.h"
#include "FileManager.h"

#include "Exports.h"

struct EntityLevelData
{
	//uint32_t levelIndex;
	std::string name = "";
	Entity parent = {};
	Entity firstChild = {};
	Entity nextSibling = {};
	Entity lastSibling = {};
	uint32_t depth = 0;
};

class LevelManager
{
private:
	static inline LevelManager* Instance;

	Event<const Entity&> onEntityCreated;
	Event<const Entity&> onEntityDestroyed;

	//std::vector<Entity> entities;
	//std::unordered_map<uint32_t, EntityLevelData> entityLevelDataMap;
	// Invalid (default) entity is mapped to the root node of the level
	std::unordered_map<Entity, EntityLevelData> entityLevelDataMap = { {}, {} };

public:
	LevelManager() {};
	~LevelManager() {};

	void Startup();
	void Shutdown();

	REDFORGE_API static Entity CreateEntity(std::string name = "", Entity parent = {});
	REDFORGE_API static void DestroyEntity(Entity entity);

	//REDFORGE_API static uint32_t GetLevelIndex(Entity entity);
	REDFORGE_API static std::string GetEntityName(Entity entity);
	REDFORGE_API static Entity GetEntityParent(Entity entity);
	REDFORGE_API static Entity GetEntityFirstChild(Entity entity);
	REDFORGE_API static Entity GetEntityNextSibling(Entity entity);
	REDFORGE_API static Entity GetEntityLastSibling(Entity entity);
	REDFORGE_API static uint32_t GetEntityDepth(Entity entity);

	REDFORGE_API static bool SetEntityParent(Entity entity, Entity newParent);
	REDFORGE_API static bool MoveEntityBefore(Entity entity, Entity next);
	REDFORGE_API static bool MoveEntityAfter(Entity entity, Entity previous);

	REDFORGE_API static bool IsEntityChildOf(Entity parent, Entity child);

	// Recursively traverses the level's entity hierarchy, performing the callback on each valid entity.
	REDFORGE_API static void ForEachEntity(std::function<void(const Entity&)> callback, Entity root = {});

	//REDFORGE_API static std::vector<Entity> GetAllEntities() { return Instance->entities; };

	REDFORGE_API static void SaveLevel();
	REDFORGE_API static void LoadLevel();

	REDFORGE_API static Event<const Entity&>* GetOnEntityCreated() { return Instance ? &Instance->onEntityCreated : nullptr; }
	REDFORGE_API static Event<const Entity&>* GetOnEntityDestroyed() { return Instance ? &Instance->onEntityDestroyed : nullptr; }

private:
	static SerializedObject SaveEntity(Entity entity);
	static void LoadEntity(const SerializedObject& entityObject, Entity parent);
};