#pragma once

#include "TimeManager.h"
#include "ResourceManager.h"
#include "GraphicsSystem.h"
#include "EntityManager.h"

class Engine
{
private:
	TimeManager timeManager;
	ResourceManager resourceManager;
	GraphicsSystem graphics;
	EntityManager entityManager;

	bool isRunning = false;

public:
	Engine() {};
	~Engine() {};

	void Run();
};