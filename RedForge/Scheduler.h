#pragma once

#include "Stage.h"

class Scheduler
{
private:
	World* world;

	std::vector<TypeID> systemTypes;
	std::unordered_map<TypeID, SystemBase*> systems;
	std::unordered_map<TypeID, size_t> systemPhaseIndices;

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

	bool IsSystemCompatibleWithPhase(TypeID systemType, const Phase& phase);

	void Update(float deltaTime);
	void RunPhase(size_t phaseIndex, float deltaTime);

	void QueueExternalTask(std::function<void(World&)> callback);

	friend class Engine;
};