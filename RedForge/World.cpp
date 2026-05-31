#include "World.h"

World::World() 
	: entityManager(this)
{
	// Sort resource order by dependencies
	/* Sort all systems in dependent order */

	std::vector<std::type_index> resourceTypesList = GetRegisteredResourcesList();
	std::unordered_map<std::type_index, size_t> resourceInDegrees;
	std::queue<std::type_index> zeroInDegreeResources;
	for (std::type_index resource : resourceTypesList)
	{
		resourceInDegrees[resource] = GET_RESOURCE_DEPENDENCIES(resource).size();
		if (resourceInDegrees[resource] == 0)
			zeroInDegreeResources.push(resource);
	}

	std::vector<std::type_index> sortedResources;
	while (!zeroInDegreeResources.empty())
	{
		std::type_index resource = zeroInDegreeResources.front();
		zeroInDegreeResources.pop();

		sortedResources.push_back(resource);
		for (std::type_index dependent : GET_RESOURCE_DEPENDENTS(resource))
		{
			resourceInDegrees[dependent]--;
			if (resourceInDegrees[dependent] == 0)
				zeroInDegreeResources.push(dependent);
		}
	}

	resourceTypes = sortedResources;

	// Instantiate all registered resources
	for(std::type_index resourceType : resourceTypes)
		resources[resourceType] = GetRegisteredResourceInfoMap()[resourceType].createResource();

	// Instantiate a queue for all registered events
	for(std::type_index eventType : GetRegisteredEventsList())
		eventQueues[eventType] = GetRegisteredEventInfoMap()[eventType].createQueue();
}
World::~World()
{

}

void World::Startup(const EngineStartupParams& params)
{
	// Startup all resources
	for(const std::type_index& resourceType : resourceTypes)
		resources[resourceType]->Startup(params, *this);

	entityManager.Startup();
}
void World::Shutdown(const EngineShutdownParams& params)
{
	// Shutdown and delete all resources
	for(const std::type_index& resourceType : resourceTypes)
	{
		resources[resourceType]->Shutdown(params, *this);
		delete resources[resourceType];
	}
	resources.clear();

	// Delete all event queues
	for(std::pair<const std::type_index, IEventQueue*>& eventQueue : eventQueues)
		delete eventQueue.second;
	eventQueues.clear();
}

void World::QueueEventOfType(std::type_index eventType, const void* event)
{
	eventQueues.at(eventType)->PushEvent(event);
}