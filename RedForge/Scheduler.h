#pragma once

#include "Stage.h"
//#include "Phase.h"

#include "SystemMacros.h"

class Scheduler
{
private:
	World* world;

	std::vector<std::type_index> systemTypes;
	std::unordered_map<std::type_index, SystemBase*> systems;

	//std::vector<Stage> stages;
	std::vector<Phase> phases;

	bool isRunningExternalTasks = false;
	std::vector<std::function<void(World&)>> externalTasks;

private:
	Scheduler();
	~Scheduler();

	void Startup(const EngineStartupParams& params, World& world);
	void Shutdown(const EngineShutdownParams& params, World& world);

	void GeneratePhases();

	bool IsSystemCompatibleWithPhase(std::type_index systemType, const Phase& phase);

	void Update(float deltaTime);
	void RunPhase(size_t phaseIndex, float deltaTime);

	void QueueExternalTask(std::function<void(World&)> callback);

	friend class Engine;
};