#include "TimeManager.h"

#include <cstdio>
#include <chrono>

void TimeManager::Startup()
{
	Instance = this;

	startTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	previousTimeMicro = 0;
	currentTimeMicro = 0;
}
void TimeManager::Shutdown()
{

}

void TimeManager::Update()
{
	static float maxDelta = 0.0f;

	currentTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - startTimeMicro;
	deltaTime = (static_cast<float>(currentTimeMicro - previousTimeMicro)) / 1000000.0f;
	previousTimeMicro = currentTimeMicro;

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