#include "Scheduler.h"

#include <unordered_set>
#include <queue>

#include "SystemMacros.h"

Scheduler::Scheduler()
{
	/* Sort all systems in dependent order */

	std::vector<std::type_index> systemTypesList = GetRegisteredSystemsList();
	std::unordered_map<std::type_index, size_t> systemInDegrees;
	std::queue<std::type_index> zeroInDegreeSystems;
	for(std::type_index system : systemTypesList)
	{
		systemInDegrees[system] = GET_SYSTEM_DEPENDENCIES(system).size();
		if(systemInDegrees[system] == 0)
			zeroInDegreeSystems.push(system);
	}

	std::vector<std::type_index> sortedSystems;
	while(!zeroInDegreeSystems.empty())
	{
		std::type_index system = zeroInDegreeSystems.front();
		zeroInDegreeSystems.pop();

		sortedSystems.push_back(system);
		for(std::type_index dependent : GET_SYSTEM_DEPENDENTS(system))
		{
			systemInDegrees[dependent]--;
			if(systemInDegrees[dependent] == 0)
				zeroInDegreeSystems.push(dependent);
		}
	}

	systemTypes = sortedSystems;

	// Instantiate all registered systems
	for(std::type_index systemType : systemTypes)
		systems[systemType] = GetRegisteredSystemInfoMap()[systemType].createSystem();
}
Scheduler::~Scheduler()
{

}

void Scheduler::Startup(const EngineStartupParams& params, World& world)
{
	this->world = &world;

	world.Startup(params);

	GeneratePhases();

	for(size_t i = 0; i < systemTypes.size(); i++)
		systems[systemTypes[i]]->Startup(params, world);
	for(size_t i = 0; i < systemTypes.size(); i++)
		systems[systemTypes[i]]->PostStartup(params, world);
}
void Scheduler::Shutdown(const EngineShutdownParams& params, World& world)
{
	// Shutdown and delete all systems
	for(size_t i = systemTypes.size() - 1; i < systemTypes.size(); i--)
	{
		SystemBase* system = systems[systemTypes[i]];
		system->Shutdown(params, world);
		delete system;
	}
	systemTypes.clear();

	// Clear phases
	for(Phase& phase : phases)
		phase.ClearSystems();
	phases.clear();

	world.Shutdown(params);
}	

void Scheduler::GeneratePhases()
{
	/* Generate phases from sorted systems list */

	for(std::type_index system : systemTypes)
	{
		size_t minPhase = 0;
		// Determine the minimum phase index this system can be placed in based on its dependencies
		for(std::type_index dependency : GET_SYSTEM_DEPENDENCIES(system))
		{
			size_t dependencyPhase = GET_SYSTEM_PHASEINDEX(dependency);
			if(dependencyPhase + 1 > minPhase)
				minPhase = dependencyPhase + 1;
		}

		// Try to place the system in an existing phase, starting from minPhase
		bool placedInPhase = false;
		for(size_t phaseIndex = minPhase; phaseIndex < phases.size(); phaseIndex++)
		{
			if(IsSystemCompatibleWithPhase(system, phases[phaseIndex]))
			{
				phases[phaseIndex].AddSystem(world, system, systems[system]);
				GetRegisteredSystemInfoMap()[system].phaseIndex = phaseIndex;
				placedInPhase = true;
				break;
			}
		}
		// If the system couldn't be placed in an existing phase, create a new phase for it
		if(GET_SYSTEM_PHASEINDEX(system) == -1)
		{
			Phase& newPhase = phases.emplace_back();
			newPhase.AddSystem(world, system, systems[system]);
		}
	}
}

bool Scheduler::IsSystemCompatibleWithPhase(std::type_index systemType, const Phase& phase)
{
	std::vector<std::type_index> systemWrites = GET_SYSTEM_WRITELIST(systemType);
	std::vector<std::type_index> systemReads = GET_SYSTEM_READLIST(systemType);
	std::vector<std::type_index> phaseWrites = phase.GetWriteList();
	std::vector<std::type_index> phaseReads = phase.GetReadList();

	// If phase writes to a component that the system also writes to, they're not compatible
	std::unordered_set<std::type_index> phaseWritesSystemWrites(phaseWrites.begin(), phaseWrites.end());
	phaseWritesSystemWrites.insert(systemWrites.begin(), systemWrites.end());
	if(phaseWritesSystemWrites.size() < phaseWrites.size() + systemWrites.size())
		return false;
	
	// If phase writes to a component that the system reads from, they're not compatible
	std::unordered_set<std::type_index> phaseWritesSystemReads(phaseWrites.begin(), phaseWrites.end());
	phaseWritesSystemReads.insert(systemReads.begin(), systemReads.end());
	if(phaseWritesSystemReads.size() < phaseWrites.size() + systemReads.size())
		return false;
	
	// If the system writes to a component that the phase reads from, they're not compatible
	std::unordered_set<std::type_index> systemWritesPhaseReads(systemWrites.begin(), systemWrites.end());
	systemWritesPhaseReads.insert(phaseReads.begin(), phaseReads.end());
	if(systemWritesPhaseReads.size() < systemWrites.size() + phaseReads.size())
		return false;

	return true;
}

void Scheduler::Update(float deltaTime)
{
	for(size_t i = 0; i < phases.size(); i++)
		RunPhase(i, deltaTime);

	// Finalize queued events by swapping the read and write buffers of all event queues
	for(const std::pair<std::type_index, IEventQueue*>& eventQueue : world->eventQueues)
		eventQueue.second->SwapBuffers();
	
	isRunningExternalTasks = true;

	for(size_t i = 0; i < externalTasks.size(); i++)
		externalTasks[i](*world);
	externalTasks.clear();
	
	isRunningExternalTasks = false;
}
void Scheduler::RunPhase(size_t phaseIndex, float deltaTime)
{
	Phase& phase = phases[phaseIndex];

	for(size_t i = 0; i < phase.GetSystemCount(); i++)
		phase.GetSystem(i)->Update(phase.GetSystemContext(i), deltaTime);
	for(size_t i = 0; i < phase.GetSystemCount(); i++)
		phase.GetSystemContext(i)->commandBuffer->Flush();
}

void Scheduler::QueueExternalTask(std::function<void(World&)> callback)
{
	// If we're already running external tasks, execute the callback immediately; otherwise, queue it for later
	if(isRunningExternalTasks)
		callback(*world);
	else
		externalTasks.push_back(callback);
}