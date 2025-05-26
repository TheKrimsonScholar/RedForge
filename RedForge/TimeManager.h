#pragma once

#include <cstdint>

class TimeManager
{
private:
	static inline TimeManager* Instance;

	long totalFrames = -1;

	double startTime = 0;
	double previousTime = 0;
	double currentTime = 0;
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

	void Update();

	static inline long GetCurrentFrame() { return Instance->totalFrames; };
	static inline double GetCurrentTime() { return Instance->currentTime; };
	static inline float GetDeltaTime() { return Instance->deltaTime; };
	static inline float GetFPS() { return 1.0f / Instance->deltaTime; };
	static inline float GetAverageFPS() { return Instance->currentFPS; };
	static inline float GetMinFPS() { return Instance->minFPS; };
};