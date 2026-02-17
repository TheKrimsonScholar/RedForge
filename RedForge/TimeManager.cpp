#include "TimeManager.h"

#include <cstdio>
#include <chrono>

REGISTER_SYSTEM(TimeManager)

TimeManager::TimeManager()
{

}
TimeManager::~TimeManager()
{

}

void TimeManager::Startup(const EngineStartupParams& params, World& world)
{
	Time& time = world.GetResource<Time>();

	time.startTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	time.previousTimeMicro = 0;
	time.currentTimeMicro = 0;
}
void TimeManager::PostStartup(const EngineStartupParams& params, World& world)
{

}
void TimeManager::Shutdown(const EngineShutdownParams& params, World& world)
{

}

void TimeManager::Update(SystemContext<Time>& ctx, float deltaTime)
{
	static float maxDelta = 0.0f;

	Time& time = ctx.GetResource<Time>();

	time.currentTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - time.startTimeMicro;
	time.deltaTime = (static_cast<float>(time.currentTimeMicro - time.previousTimeMicro)) / 1000000.0f;
	time.previousTimeMicro = time.currentTimeMicro;

	time.totalFrames++;
	time.numFrames++;
	time.accumulatedTime += deltaTime;

	if(deltaTime > maxDelta)
		maxDelta = deltaTime;

	if(time.accumulatedTime > time.averageInterval)
	{
		time.currentFPS = static_cast<float>(time.numFrames / time.accumulatedTime);
		time.minFPS = 1.0f / maxDelta;
		maxDelta = 0.0f;

		time.numFrames = 0;
		time.accumulatedTime = 0;
	}
}