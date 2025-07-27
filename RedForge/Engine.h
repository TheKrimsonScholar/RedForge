#pragma once

#include "TimeManager.h"
#include "ResourceManager.h"
#include "InputSystem.h"
#include "GraphicsSystem.h"
#include "EntityManager.h"
#include "CameraManager.h"
#include "DebugManager.h"
#include "PhysicsSystem.h"
#include "FileManager.h"
#include "LevelManager.h"

#include "Exports.h"

REDFORGE_API class Engine
{
private:
	TimeManager timeManager;
	ResourceManager resourceManager;
	InputSystem inputSystem;
	GraphicsSystem graphics;
	EntityManager entityManager;
	CameraManager cameraManager;
	DebugManager debugManager;
	PhysicsSystem physicsSystem;
	FileManager fileManager;
	LevelManager levelManager;

	bool isRunning = false;

public:
	Engine() {};
	~Engine() {};

	REDFORGE_API void Run();

	REDFORGE_API void Startup(bool shouldOverrideFramebuffer = false, unsigned int overrideExtentWidth = 0, unsigned int overrideExtentHeight = 0);
	REDFORGE_API void Shutdown();

	REDFORGE_API void Update();

	/*REDFORGE_API TimeManager& GetTimeManager() { return timeManager; };
	REDFORGE_API InputSystem& GetInputSystem() { return inputSystem; };
	REDFORGE_API GraphicsSystem& GetGraphicsSystem() { return graphics; };
	REDFORGE_API PhysicsSystem& GetPhysicsSystem() { return physicsSystem; };*/

	REDFORGE_API bool IsRunning() { return isRunning; };
};