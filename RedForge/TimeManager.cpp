#include "TimeManager.h"

#include <cstdio>
#include <chrono>

#include <GLFW/glfw3.h>

void TimeManager::Startup()
{
	Instance = this;

	startTimeMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	previousTimeMillis = 0;
	currentTimeMillis = 0;
}
void TimeManager::Shutdown()
{

}

void TimeManager::Update()
{
	static float maxDelta = 0.0f;

	currentTimeMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startTimeMillis;
	deltaTime = (currentTimeMillis - previousTimeMillis) / 1000.0f;
	previousTimeMillis = currentTimeMillis;

	totalFrames++;
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