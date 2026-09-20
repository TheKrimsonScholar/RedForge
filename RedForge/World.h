#pragma once

#include <unordered_map>
#include <typeindex>

#include "EngineParams.h"
#include "TypeRegistry.h"
#include "EventQueue.h"
#include "EntityManager.h"

class World
{
private:
	std::unordered_map<TypeID, IResource*> resources;

	std::unordered_map<TypeID, IEventQueue*> eventQueues;

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

	template<typename T>
	void QueueEvent(const T& event);
	void QueueEventOfType(TypeID eventType, const void* event);
	template<typename T>
	const std::vector<T>& PollEvents() const;

	EntityManager& GetEntityManager() { return entityManager; }

	friend class Engine;
	friend class Scheduler;
};

template<typename T>
inline void World::QueueEvent(const T& event)
{
	static_cast<EventQueue<T>*>(eventQueues.at(TYPEID(T)))->PushEvent(event);
}
template<typename T>
inline const std::vector<T>& World::PollEvents() const
{
	return static_cast<EventQueue<T>*>(eventQueues.at(TYPEID(T)))->PollEvents();
}