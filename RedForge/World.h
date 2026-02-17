#pragma once

#include <unordered_map>
#include <typeindex>

#include "EngineParams.h"
#include "ResourceMacros.h"
#include "EventQueue.h"
#include "EntityManager.h"

class World
{
private:
	std::vector<std::type_index> resourceTypes;
	std::unordered_map<std::type_index, IResource*> resources;

	std::unordered_map<std::type_index, IEventQueue*> eventQueues;

	EntityManager entityManager;

private:
	World();
	~World();

	void Startup(const EngineStartupParams& params);
	void Shutdown(const EngineShutdownParams& params);

	void ResolveResourceDependencies();

public:
	template<typename T>
	T& GetResource() const { return *static_cast<T*>(resources.at(typeid(T))); }

	template<typename T>
	void QueueEvent(const T& event);
	void QueueEventOfType(std::type_index eventType, const void* event);
	template<typename T>
	const std::vector<T>& PollEvents() const;

	EntityManager& GetEntityManager() { return entityManager; }

	friend class Engine;
	friend class Scheduler;
};

template<typename T>
inline void World::QueueEvent(const T& event)
{
	static_cast<EventQueue<T>*>(eventQueues.at(typeid(T)))->PushEvent(event);
}
template<typename T>
inline const std::vector<T>& World::PollEvents() const
{
	return static_cast<EventQueue<T>*>(eventQueues.at(typeid(T)))->PollEvents();
}