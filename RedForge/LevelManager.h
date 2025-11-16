#pragma once

#include <fstream>

#include "Event.h"
#include "EntityManager.h"
#include "FileManager.h"

#include "Exports.h"

struct EntityLevelData
{
	std::string name = "";
	// The file path of the prefab which this entity is the root of. Default value if this entity is not a prefab.
	std::filesystem::path prefabPath = {};
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
	Event<const Entity&> onEntityLevelDataModified;
	Event<const Entity&, const Entity&> onEntityReparented;
	Event<const Entity&, const Entity&> onEntityMovedBefore;
	Event<const Entity&, const Entity&> onEntityMovedAfter;

	//std::vector<Entity> entities;
	//std::unordered_map<uint32_t, EntityLevelData> entityLevelDataMap;
	// Invalid (default) entity is mapped to the root node of the level
	std::unordered_map<Entity, EntityLevelData> entityLevelDataMap = { {}, {} };

	std::unordered_map<std::filesystem::path, SerializedObject> activePrefabs;

public:
	LevelManager() {};
	~LevelManager() {};

	void Startup();
	void Shutdown();

	REDFORGE_API static Entity CreateEntity(std::string name = "", Entity parent = {}, const std::filesystem::path& prefabPath = {});
	REDFORGE_API static void DestroyEntity(Entity entity);

	REDFORGE_API static bool SetEntityParent(Entity entity, Entity newParent);
	REDFORGE_API static bool MoveEntityBefore(Entity entity, Entity next);
	REDFORGE_API static bool MoveEntityAfter(Entity entity, Entity previous);

	// Recursively traverses the level's entity hierarchy, performing the callback on each valid entity.
	REDFORGE_API static void ForEachEntity(std::function<void(const Entity&)> callback, Entity root = {});
	// Recursively traverses the level's entity hierarchy, performing the callback on each valid entity in reverse order, ensuring all children are processed before their parents.
	REDFORGE_API static void ForEachEntity_Reversed(std::function<void(const Entity&)> callback, Entity root = {});

	REDFORGE_API static void SaveEntityAsPrefab(const Entity& entity, const std::filesystem::path& prefabPath);
	REDFORGE_API static Entity LoadEntityFromPrefab(const std::filesystem::path& prefabPath, const Entity& parent);

	REDFORGE_API static void SaveLevel(const std::filesystem::path& levelPath);
	REDFORGE_API static void LoadLevel(const std::filesystem::path& levelPath);

private:
	static SerializedObject SaveEntity(Entity entity);
	static Entity LoadEntity(const SerializedObject& entityObject, Entity parent);

public:
	// Returns all ancestors of the entity, starting with its immediate parent and ending with the top-level entity under the level root.
	REDFORGE_API static std::vector<Entity> GetAllEntityAncestors(const Entity& entity);
	REDFORGE_API static bool IsEntityAncestorContainedInList(const Entity& entity, const std::vector<Entity>& list);

	REDFORGE_API static bool IsEntityChildOf(Entity parent, Entity child);

	//REDFORGE_API static uint32_t GetLevelIndex(Entity entity);
	REDFORGE_API static std::string GetEntityName(Entity entity);
	REDFORGE_API static std::filesystem::path GetEntityPrefabPath(Entity entity);
	REDFORGE_API static Entity GetEntityParent(Entity entity);
	REDFORGE_API static Entity GetEntityFirstChild(Entity entity);
	REDFORGE_API static Entity GetEntityNextSibling(Entity entity);
	REDFORGE_API static Entity GetEntityLastSibling(Entity entity);
	REDFORGE_API static uint32_t GetEntityDepth(Entity entity);

	REDFORGE_API static void SetEntityName(const Entity& entity, const std::string& name);

	// Triggers right after a new entity is created. Provides the created entity.
	REDFORGE_API static Event<const Entity&>* GetOnEntityCreated() { return Instance ? &Instance->onEntityCreated : nullptr; }
	// Triggers right before an entity is destroyed. Provides the entity to be destroyed.
	REDFORGE_API static Event<const Entity&>* GetOnEntityDestroyed() { return Instance ? &Instance->onEntityDestroyed : nullptr; }
	// Triggers right after an entity's level data is modified in any way, not including creation or destruction. Provides the corresponding entity.
	REDFORGE_API static Event<const Entity&>* GetOnEntityLevelDataModified() { return Instance ? &Instance->onEntityLevelDataModified : nullptr; }
	// Triggers right before an entity is parented to another entity. Provides the reparented entity and the new parent entity.
	REDFORGE_API static Event<const Entity&, const Entity&>* GetOnEntityReparented() { return Instance ? &Instance->onEntityReparented : nullptr; }
	// Triggers right before an entity is moved to the position right before another entity. Provides the moved entity and its new next sibling entity.
	REDFORGE_API static Event<const Entity&, const Entity&>* GetOnEntityMovedBefore() { return Instance ? &Instance->onEntityMovedBefore : nullptr; }
	// Triggers right before an entity is moved to the position right after another entity. Provides the moved entity and its new previous sibling entity.
	REDFORGE_API static Event<const Entity&, const Entity&>* GetOnEntityMovedAfter() { return Instance ? &Instance->onEntityMovedAfter : nullptr; }
};