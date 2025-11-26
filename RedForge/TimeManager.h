#pragma once

#include <cstdint>

#include "Exports.h"

class TimeManager
{
private:
	static inline TimeManager* Instance;

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

public:
	TimeManager() {};
	~TimeManager() {};

	void Startup();
	void Shutdown();

	REDFORGE_API void Update();

	static inline REDFORGE_API long GetCurrentFrame() { return Instance->totalFrames; };
	static inline REDFORGE_API double GetCurrentTime() { return Instance->currentTimeMicro / 1000000.0; };
	static inline REDFORGE_API float GetDeltaTime() { return Instance->deltaTime; };
	static inline REDFORGE_API float GetFPS() { return 1.0f / Instance->deltaTime; };
	static inline REDFORGE_API float GetAverageFPS() { return Instance->currentFPS; };
	static inline REDFORGE_API float GetMinFPS() { return Instance->minFPS; };
};