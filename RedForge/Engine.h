#pragma once

#include "GraphicsSystem.h"
#include "TimeManager.h"

class Engine
{
private:
	GraphicsSystem graphics;
	TimeManager timeManager;

	bool isRunning = false;

public:
	Engine() {};
	~Engine() {};

	void Run();
};