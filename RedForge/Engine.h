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
#include "NetworkSystem.h"

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
	NetworkSystem networkSystem;

	bool isRunning = false;

public:
	Engine() {};
	~Engine() {};

	REDFORGE_API void Run();

	// Create the Vulkan instance to be used by the graphics system. Should only be called if the instance is needed before engine initialization.
	REDFORGE_API void CreateVulkanInstance();
	REDFORGE_API void DestroyVulkanInstance();

	REDFORGE_API void Startup(VkSurfaceKHR surfaceOverride = VK_NULL_HANDLE);
	REDFORGE_API void Shutdown(bool shouldDestroyVulkanInstance = true);

	REDFORGE_API void Update();

	REDFORGE_API bool IsRunning() { return isRunning; };
};