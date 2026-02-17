#pragma once

#include "ResourceRegistrationMacros.h"

#include <cstdint>

struct Time : public IResource
{
	long totalFrames = -1;

	long long startTimeMicro = 0;
	long long previousTimeMicro = 0;
	long long currentTimeMicro = 0;
	float deltaTime = 0;

	float averageInterval = 0.5f;
	uint32_t numFrames = 0;
	double accumulatedTime = 0;
	float currentFPS = 0.0f;
	float minFPS = 0.0f;

private:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

public:
	long GetCurrentFrame() const { return totalFrames; };
	double GetCurrentTime() const { return currentTimeMicro / 1000000.0; };
	float GetFPS() const { return 1.0f / deltaTime; };
	float GetAverageFPS() const { return currentFPS; };
};
REGISTER_RESOURCE(Time)