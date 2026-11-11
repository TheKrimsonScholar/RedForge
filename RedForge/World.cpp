#include "World.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

World::World() 
	: entityManager(this)
{
	// Instantiate all registered resources in dependent order
	for(TypeID resourceType : GET_ALL_RESOURCE_TYPES)
		resources[resourceType] = TypeRegistry::GetRegisteredResourceInfoMap().at(resourceType).createResource();

	// Instantiate a queue for all registered events
	for(TypeID eventType : TypeRegistry::GetRegisteredEventsList())
		eventQueues[eventType] = TypeRegistry::GetRegisteredEventInfoMap().at(eventType).createQueue();
}
World::~World()
{
	// Destroy all resources
	for(TypeID resourceType : GET_ALL_RESOURCE_TYPES)
		delete resources[resourceType];
	resources.clear();
}

void World::Startup(const EngineStartupParams& params)
{
	// Startup all resources
	for(TypeID resourceType : GET_ALL_RESOURCE_TYPES)
		resources[resourceType]->Startup(params, *this);

	entityManager.Startup(params);
}
void World::Shutdown(const EngineShutdownParams& params)
{
	entityManager.Shutdown(params);

	// Shutdown all resources
	for(TypeID resourceType : GET_ALL_RESOURCE_TYPES)
		resources[resourceType]->Shutdown(params, *this);

	// Delete all event queues
	for(const std::pair<TypeID, IEventQueue*>& eventQueue : eventQueues)
		delete eventQueue.second;
	eventQueues.clear();
}

void World::QueueEventOfType(TypeID eventType, const void* event)
{
	eventQueues.at(eventType)->PushEvent(event, totalEventCount);
	totalEventCount++;
}