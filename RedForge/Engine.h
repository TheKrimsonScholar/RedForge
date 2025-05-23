#pragma once

#include "TimeManager.h"
#include "ResourceManager.h"
#include "GraphicsSystem.h"
#include "EntityManager.h"
#include "CameraManager.h"

class Engine
{
private:
	TimeManager timeManager;
	ResourceManager resourceManager;
	GraphicsSystem graphics;
	EntityManager entityManager;
	CameraManager cameraManager;

	bool isRunning = false;

public:
	Engine() {};
	~Engine() {};

	void Run();
};