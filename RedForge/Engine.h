#pragma once

#include "TimeManager.h"
#include "ResourceManager.h"
#include "InputSystem.h"
#include "GraphicsSystem.h"
#include "EntityManager.h"
#include "CameraManager.h"
#include "DebugManager.h"

class Engine
{
private:
	TimeManager timeManager;
	ResourceManager resourceManager;
	InputSystem inputSystem;
	GraphicsSystem graphics;
	EntityManager entityManager;
	CameraManager cameraManager;
	DebugManager debugManager;

	bool isRunning = false;

public:
	Engine() {};
	~Engine() {};

	void Run();
};