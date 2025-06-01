#pragma once

#include "TimeManager.h"
#include "ResourceManager.h"
#include "InputSystem.h"
#include "GraphicsSystem.h"
#include "EntityManager.h"
#include "CameraManager.h"
#include "DebugManager.h"
#include "PhysicsSystem.h"

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

	bool isRunning = false;

public:
	Engine() {};
	~Engine() {};

	REDFORGE_API void Run();

	REDFORGE_API void Startup();
	REDFORGE_API void Shutdown();

	REDFORGE_API TimeManager& GetTimeManager() { return timeManager; };
	REDFORGE_API InputSystem& GetInputSystem() { return inputSystem; };
	REDFORGE_API GraphicsSystem& GetGraphicsSystem() { return graphics; };
	REDFORGE_API PhysicsSystem& GetPhysicsSystem() { return physicsSystem; };
};