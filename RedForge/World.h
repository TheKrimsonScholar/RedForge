#pragma once

#include <unordered_map>
#include <typeindex>

#include "EngineParams.h"
#include "TypeRegistry.h"
#include "EventQueue.h"
#include "GenericEventData.h"
#include "EntityManager.h"

class World
{
private:
	std::unordered_map<TypeID, IResource*> resources;

	std::unordered_map<TypeID, IEventQueue*> eventQueues;
	size_t totalEventCount = 0;

	EntityManager entityManager;

private:
	World();
	~World();

	void Startup(const EngineStartupParams& params);
	void Shutdown(const EngineShutdownParams& params);

	void ResolveResourceDependencies();

public:
	template<typename T>
	T& GetResource() const { return *static_cast<T*>(resources.at(TypeRegistry::GetTypeID<T>())); }
	void* GetResourceOfType(TypeID resourceType) const { return resources.at(resourceType); }

	template<typename T>
	void QueueEvent(const T& event);
	void QueueEventOfType(TypeID eventType, const void* event);
	template<typename T>
	const std::vector<T>& PollEvents() const;
	template<typename... Ts>
	void PollEvents(std::vector<GenericEventData>& out_eventList) const;

	EntityManager& GetEntityManager() { return entityManager; }

	friend class Engine;
	friend class Scheduler;
};

template<typename T>
inline void World::QueueEvent(const T& event)
{
	static_cast<EventQueue<T>*>(eventQueues.at(TYPEID(T)))->PushEvent(event, totalEventCount);
	totalEventCount++;
}
template<typename T>
inline const std::vector<T>& World::PollEvents() const
{
	return static_cast<EventQueue<T>*>(eventQueues.at(TYPEID(T)))->PollEvents();
}
template<typename... Ts>
inline void World::PollEvents(std::vector<GenericEventData>& out_eventList) const
{
	std::vector<TypeID> remainingEventTypes = { TYPEID(Ts)... };
	// Ignore all event types with an empty event queue
	for(size_t i = 0; i < remainingEventTypes.size(); i++)
		if(eventQueues.at(remainingEventTypes[i])->IsEmpty())
			remainingEventTypes.erase(remainingEventTypes.begin() + i--);

	std::vector<const void*> remainingEventLists;
	std::vector<std::vector<size_t>> remainingIndexLists;
	std::vector<size_t> nextEventCounters; // The local index of the next event to be processed in each remaining event queue
	for(TypeID eventType : remainingEventTypes)
	{
		remainingEventLists.push_back(eventQueues.at(eventType)->GetRawEventListPtr());
		remainingIndexLists.push_back(eventQueues.at(eventType)->PollEventIndices());
		nextEventCounters.push_back(0);
	}

	// Each iteration, find the event queue with the smallest next absolute event index and add that event to the output list
	while(!remainingEventLists.empty())
	{
		size_t minQueueIndex = ULLONG_MAX; // Index of the queue whose next event has the lowest absolute index of the queues remaining
		size_t minEventIndex = ULLONG_MAX; // The lowest absolute event index among the queues remaining
		for(size_t i = 0; i < nextEventCounters.size(); i++)
		{
			size_t nextAbsoluteIndex = remainingIndexLists[i][nextEventCounters[i]];
			if(nextAbsoluteIndex < minEventIndex)
			{
				minQueueIndex = i;
				minEventIndex = nextAbsoluteIndex;
			}
		}

		// Take the next event from this queue and copy it to the complete ordered list
		TypeID eventType = remainingEventTypes[minQueueIndex];
		const void* nextEventPtr = reinterpret_cast<const uint8_t*>(remainingEventLists[minQueueIndex]) + GET_TYPE_SIZE(eventType) * nextEventCounters[minQueueIndex];
		out_eventList.emplace_back(eventType, nextEventPtr);

		// Move on to the next event for this queue
		nextEventCounters[minQueueIndex]++;
		// If all events from this queue have been added, remove it from the list of remaining queues
		if(nextEventCounters[minQueueIndex] >= remainingIndexLists[minQueueIndex].size())
		{
			remainingEventTypes.erase(remainingEventTypes.begin() + minQueueIndex);
			remainingEventLists.erase(remainingEventLists.begin() + minQueueIndex);
			remainingIndexLists.erase(remainingIndexLists.begin() + minQueueIndex);
			nextEventCounters.erase(nextEventCounters.begin() + minQueueIndex);
		}
	}
}