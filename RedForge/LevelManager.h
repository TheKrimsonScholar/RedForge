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
	Event<const Entity&, const Entity&> onEntityReparented;
	Event<const Entity&, const Entity&> onEntityMovedBefore;
	Event<const Entity&, const Entity&> onEntityMovedAfter;

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

	REDFORGE_API static bool SetEntityParent(Entity entity, Entity newParent);
	REDFORGE_API static bool MoveEntityBefore(Entity entity, Entity next);
	REDFORGE_API static bool MoveEntityAfter(Entity entity, Entity previous);

	// Recursively traverses the level's entity hierarchy, performing the callback on each valid entity.
	REDFORGE_API static void ForEachEntity(std::function<void(const Entity&)> callback, Entity root = {});
	// Recursively traverses the level's entity hierarchy, performing the callback on each valid entity in reverse order, ensuring all children are processed before their parents.
	REDFORGE_API static void ForEachEntity_Reversed(std::function<void(const Entity&)> callback, Entity root = {});

	REDFORGE_API static void SaveLevel();
	REDFORGE_API static void LoadLevel();

private:
	static SerializedObject SaveEntity(Entity entity);
	static void LoadEntity(const SerializedObject& entityObject, Entity parent);

public:
	//REDFORGE_API static std::vector<Entity> GetAllEntities() { return Instance->entities; };
	
	// Returns all ancestors of the entity, starting with its immediate parent and ending with the top-level entity under the level root.
	REDFORGE_API static std::vector<Entity> GetAllEntityAncestors(const Entity& entity);
	REDFORGE_API static bool IsEntityAncestorContainedInList(const Entity& entity, const std::vector<Entity>& list);

	REDFORGE_API static bool IsEntityChildOf(Entity parent, Entity child);

	//REDFORGE_API static uint32_t GetLevelIndex(Entity entity);
	REDFORGE_API static std::string GetEntityName(Entity entity);
	REDFORGE_API static Entity GetEntityParent(Entity entity);
	REDFORGE_API static Entity GetEntityFirstChild(Entity entity);
	REDFORGE_API static Entity GetEntityNextSibling(Entity entity);
	REDFORGE_API static Entity GetEntityLastSibling(Entity entity);
	REDFORGE_API static uint32_t GetEntityDepth(Entity entity);

	// Triggers right after a new entity is created. Provides the created entity.
	REDFORGE_API static Event<const Entity&>* GetOnEntityCreated() { return Instance ? &Instance->onEntityCreated : nullptr; }
	// Triggers right before an entity is destroyed. Provides the entity to be destroyed.
	REDFORGE_API static Event<const Entity&>* GetOnEntityDestroyed() { return Instance ? &Instance->onEntityDestroyed : nullptr; }
	// Triggers right before an entity is parented to another entity. Provides the reparented entity and the new parent entity.
	REDFORGE_API static Event<const Entity&, const Entity&>* GetOnEntityReparented() { return Instance ? &Instance->onEntityReparented : nullptr; }
	// Triggers right before an entity is moved to the position right before another entity. Provides the moved entity and its new next sibling entity.
	REDFORGE_API static Event<const Entity&, const Entity&>* GetOnEntityMovedBefore() { return Instance ? &Instance->onEntityMovedBefore : nullptr; }
	// Triggers right before an entity is moved to the position right after another entity. Provides the moved entity and its new previous sibling entity.
	REDFORGE_API static Event<const Entity&, const Entity&>* GetOnEntityMovedAfter() { return Instance ? &Instance->onEntityMovedAfter : nullptr; }
};