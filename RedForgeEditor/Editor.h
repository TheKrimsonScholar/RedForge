#pragma once

class Engine;
class World;
class EntityManager;

class Editor
{
	static inline Engine* engine;

public:
	static Engine* CreateEngineInstance();
	static void DestroyEngineInstance();

	static Engine* GetEngine();
	static World& GetWorld();
	static EntityManager& GetEntityManager();
};