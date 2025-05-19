#include "TimeManager.h"

#include <cstdio>

#include <GLFW/glfw3.h>

void TimeManager::Startup()
{
	Instance = this;

	startTime = glfwGetTime();
	previousTime = startTime;
	currentTime = startTime;
}
void TimeManager::Shutdown()
{

}

void TimeManager::Update()
{
	static float maxDelta = 0.0f;

	currentTime = glfwGetTime();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	numFrames++;
	accumulatedTime += deltaTime;

	if(deltaTime > maxDelta)
		maxDelta = deltaTime;

	if(accumulatedTime > averageInterval)
	{
		currentFPS = static_cast<float>(numFrames / accumulatedTime);
		minFPS = 1.0f / maxDelta;
		maxDelta = 0.0f;

		numFrames = 0;
		accumulatedTime = 0;
	}
}