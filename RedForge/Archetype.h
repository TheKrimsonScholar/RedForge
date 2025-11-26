#pragma once

#include "System.h"

#include "LevelManager.h"

#include "ComponentRegistrationMacros.h"

#include "TransformComponent.h"

#include <functional>

struct PlayerComponent {};
REGISTER_COMPONENT_BEGIN(PlayerComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VARS_END
REGISTER_COMPONENT_END(PlayerComponent)

struct EnemyComponent {};
REGISTER_COMPONENT_BEGIN(EnemyComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VARS_END
REGISTER_COMPONENT_END(EnemyComponent)

#define ARCHETYPE_PREUPDATE_EVENTS(...) struct ArchetypePreUpdateEvents : public Archetype::ArchetypePreUpdateEvents<__VA_ARGS__> {};
#define ARCHETYPE_POSTUPDATE_EVENTS(...) struct ArchetypePostUpdateEvents : public Archetype::ArchetypePostUpdateEvents<__VA_ARGS__> {};

#define ARCHETYPE_EVENT(Archetype, EventType)										\
REDFORGE_API static void On##EventType(const Entity& entity, EventType& event);		\
static void Queue##EventType(const Entity& entity, const EventType& event)			\
{																					\
	EntityManager::AddComponent<EventType>(entity, event);							\
}																					\
static inline class RegisterArchetypeEvent_##EventType								\
{																					\
public:																				\
	RegisterArchetypeEvent_##EventType()											\
	{																				\
		Archetype::archetypeEvents.push_back(										\
			[]()																	\
			{																		\
				EntityManager::ForEachComponentOfType<EventType>(&On##EventType);	\
				EntityManager::ForEachComponentOfType<EventType>(					\
					[](const Entity& entity, EventType& event)						\
					{																\
						EntityManager::RemoveComponent<EventType>(entity);			\
					});																\
			});																		\
	}																				\
} registerArchetypeEvent_##EventType;

template<typename T, typename... Components>
class Archetype : public System
{
protected:
	static inline T* Instance;

	static inline std::vector<std::function<void()>> archetypeEvents;

public:
	virtual Entity ConstructEntity(const Entity& parent)
	{
		return LevelManager::CreateEntity<Components...>("", parent);
	}

protected:
	void Startup() override
	{
		Instance = static_cast<T*>(this);

		EntityManager::ForEachComponentOfType<Components...>(&T::InitializeEntity);
	}
	void Shutdown() override
	{
		
	}
	void Update() override
	{
		EntityManager::ForEachComponentOfType<Components...>(&T::UpdateEntity);
		
		for(const std::function<void()>& event : T::archetypeEvents)
			event();
	}

	friend class Engine;

public:
	static T* Get() { return Instance; }
};

struct TransformMoveEvent
{
	glm::vec3 delta;
};
struct TransformRotateEvent
{

};
struct TransformScaleEvent
{

};

class Transform : public Archetype<Transform, TransformComponent>
{
//public:
//	ARCHETYPE_EVENT(Transform, TransformMoveEvent)
//	ARCHETYPE_EVENT(Transform, TransformRotateEvent)
//	ARCHETYPE_EVENT(Transform, TransformScaleEvent)

public:
	// This can stay virtual since it's only called on creation
	virtual Entity ConstructEntity(const Entity& parent) override
	{
		Entity entity = Archetype<Transform, TransformComponent>::ConstructEntity(parent);

		// Create default children, etc.

		return entity;
	}

protected:
	static void InitializeEntity(const Entity& entity, TransformComponent& player)
	{

	}
	static void UpdateEntity(const Entity& entity, TransformComponent& player)
	{

	}

	friend Archetype<Transform, TransformComponent>;
};

//struct MovementComponent
//{
//	glm::vec3 velocity;
//	glm::vec3 angularVelocity;
//	glm::vec3 acceleration;
//	glm::vec3 angularAcceleration;
//};
//
//class DynamicObject : public Archetype<DynamicObject, MovementComponent>
//{
//public:
//	ARCHETYPE_EVENT(DynamicObject, MoveEvent)
//	ARCHETYPE_EVENT(DynamicObject, AccelerateEvent)
//	ARCHETYPE_EVENT(DynamicObject, RotateEvent)
//	ARCHETYPE_EVENT(DynamicObject, AccelerateRotationEvent)
//	ARCHETYPE_EVENT(DynamicObject, ScaleEvent)
//
//public:
//	// This can stay virtual since it's only called on creation
//	virtual Entity ConstructEntity(const Entity& parent) override
//	{
//		Entity entity = Archetype::ConstructEntity(parent);
//
//		// Create default children, etc.
//
//		return entity;
//	}
//
//protected:
//	static void InitializeEntity(const Entity& entity, PlayerComponent& player)
//	{
//
//	}
//	static void UpdateEntity(const Entity& entity, PlayerComponent& player)
//	{
//
//	}
//
//	friend class Archetype;
//};

class Player : public Archetype<Player, PlayerComponent>
{
public:
	struct PlayerEvent1 { float k; };
	REGISTER_COMPONENT(PlayerEvent1)
	ARCHETYPE_EVENT(Player, PlayerEvent1)

	struct PlayerEvent2 {};
	REGISTER_COMPONENT(PlayerEvent2)
	ARCHETYPE_EVENT(Player, PlayerEvent2)

public:
	// This can stay virtual since it's only called on creation
	virtual Entity ConstructEntity(const Entity& parent) override
	{
		Entity entity = Archetype::ConstructEntity(parent);

		// Create default children, etc.

		return entity;
	}

protected:
	static void InitializeEntity(const Entity& entity, PlayerComponent& player)
	{
		
	}
	static void UpdateEntity(const Entity& entity, PlayerComponent& player)
	{
		
	}

	friend class Archetype;
};















class Enemy : public Archetype<Enemy, EnemyComponent>
{
public:
	struct PlayerEvent1 { float k; };
	REGISTER_COMPONENT(PlayerEvent1)
	ARCHETYPE_EVENT(Enemy, PlayerEvent1)

public:
	// This can stay virtual since it's only called on creation
	virtual Entity ConstructEntity(const Entity& parent) override
	{
		Entity entity = Archetype<Enemy, EnemyComponent>::ConstructEntity(parent);

		// Create default children, etc.

		return entity;
	}

protected:
	static void InitializeEntity(const Entity& entity, EnemyComponent& player)
	{
		
	}
	static void UpdateEntity(const Entity& entity, EnemyComponent& player)
	{
		
	}

	friend class Archetype<Enemy, EnemyComponent>;
};