#pragma once

#include "World.h"
#include "CommandBuffer.h"
#include "EventQueue.h"

template<typename T, typename... Ts>
struct is_one_of : std::bool_constant<((std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<Ts>> && (!std::is_const_v<Ts> || std::is_const_v<T>)) || ...)> {};
template<typename T, typename... Ts>
inline constexpr bool is_one_of_v = is_one_of<T, Ts...>::value;

template<typename... Components>
struct SystemContext;
struct SystemContextBase
{
protected:
	World* world;

	CommandBuffer* commandBuffer;
	//std::unordered_map<std::type_index, IEventQueue*> eventQueues; Instead of per-context queues, defer event queueing via command buffer

protected:
	SystemContextBase(World* world, CommandBuffer* commandBuffer) : world(world), commandBuffer(commandBuffer) {}

	template<typename... Args>
	SystemContext<Args...> ConstructContext()
	{
		return SystemContext<Args...>(world, commandBuffer);
	}

	friend class Scheduler;
};
template<typename... Components>
class System;
template<typename... Components>
struct SystemContext : public SystemContextBase
{
protected:
	SystemContext(World* world, CommandBuffer* commandBuffer) : SystemContextBase(world, commandBuffer) {}

public:
	Entity CreateEntity(const std::string& name, const Entity& parent, const std::filesystem::path& prefabPath);
	void DestroyEntity(const Entity& entity);
	
	void RenameEntity(const Entity& entity, const std::string& name);
	void ReparentEntity(const Entity& entity, const Entity& parent);
	void MoveEntityBefore(const Entity& entity, const Entity& next);
	void MoveEntityAfter(const Entity& entity, const Entity& previous);

	template<typename T>
	T& GetResource();
	
	template<typename T>
	void AddComponent(const Entity& entity, const T& component);
	template<typename T>
	void RemoveComponent(const Entity& entity);

	template<typename T>
	bool HasComponent(const Entity& entity);
	template<typename... Args>
	bool HasComponents(const Entity& entity);
	
	template<typename T>
	T& GetComponent(const Entity& entity);

	template<typename... Args, typename Callback>
	// Efficiently iterates through all active entities which have all of the specified component types. Triggers the callback for each, providing the entity along with each of its relevant components by reference.
	void ForEachComponentOfType(Callback&& callback);
	void ForEachEntity(std::function<void(const Entity&)> callback);
	void ForEachEntity(std::function<void(const Entity&)> callback, LevelID level);
	void ForEachEntity(std::function<void(const Entity&)> callback, const Entity& root);

	template<typename T>
	void QueueEvent(const T& event);
	template<typename T>
	const std::vector<T>& PollEvents() const;

	bool IsEntityActive(Entity entity);
	std::string GetEntityName(Entity entity);
	LevelID GetEntityLevel(Entity entity);
	std::filesystem::path GetEntityPrefabPath(Entity entity);
	Entity GetEntityParent(Entity entity);
	Entity GetEntityFirstChild(Entity entity);
	Entity GetEntityNextSibling(Entity entity);
	Entity GetEntityLastSibling(Entity entity);
	uint32_t GetEntityDepth(Entity entity);

	template<typename... Args>
	SystemContext<Args...> GetContext();

	friend class Scheduler;
	friend class System<Components...>;
	friend class SystemContextBase;
};

template<typename ...Components>
inline Entity SystemContext<Components...>::CreateEntity(const std::string& name, const Entity& parent, const std::filesystem::path& prefabPath)
{
	return commandBuffer->Command_CreateEntity(name, parent, prefabPath);
}
template<typename ...Components>
inline void SystemContext<Components...>::DestroyEntity(const Entity& entity)
{
	commandBuffer->Command_DestroyEntity(entity);
}

template<typename ...Components>
inline void SystemContext<Components...>::RenameEntity(const Entity& entity, const std::string& name)
{
	commandBuffer->Command_RenameEntity(entity, name);
}
template<typename ...Components>
inline void SystemContext<Components...>::ReparentEntity(const Entity& entity, const Entity& parent)
{
	return commandBuffer->Command_ReparentEntity(entity, parent);
}
template<typename ...Components>
inline void SystemContext<Components...>::MoveEntityBefore(const Entity& entity, const Entity& next)
{
	return commandBuffer->Command_MoveEntityBefore(entity, next);
}
template<typename ...Components>
inline void SystemContext<Components...>::MoveEntityAfter(const Entity& entity, const Entity& previous)
{
	return commandBuffer->Command_MoveEntityAfter(entity, previous);
}

template<typename ...Components>
template<typename T>
inline T& SystemContext<Components...>::GetResource()
{
	static_assert(is_one_of_v<T, Components...> && "Component not compatible with context signature.");

	return world->GetResource<T>();
}

template<typename ...Components>
template<typename T>
inline void SystemContext<Components...>::AddComponent(const Entity& entity, const T& component)
{
	commandBuffer->Command_AddComponent(entity, typeid(T), &component);
}
template<typename ...Components>
template<typename T>
inline void SystemContext<Components...>::RemoveComponent(const Entity& entity)
{
	commandBuffer->Command_RemoveComponent(entity, typeid(T));
}

template<typename ...Components>
template<typename T>
inline bool SystemContext<Components...>::HasComponent(const Entity& entity)
{
	return world->GetEntityManager().HasComponent<T>(entity);
}
template<typename ...Components>
template<typename ...Args>
inline bool SystemContext<Components...>::HasComponents(const Entity& entity)
{
	return world->GetEntityManager().HasComponent<Args...>(entity);
}

template<typename ...Components>
template<typename T>
inline T& SystemContext<Components...>::GetComponent(const Entity& entity)
{
	static_assert(is_one_of_v<T, Components...> && "Component not compatible with context signature.");

	return world->GetEntityManager().GetComponent<T>(entity);
}

template<typename ...Components>
template<typename ...Args, typename Callback>
inline void SystemContext<Components...>::ForEachComponentOfType(Callback&& callback)
{
	static_assert(((is_one_of_v<Args, Components...>) && ...) && "Component set not compatible with context signature.");

	world->GetEntityManager().ForEachComponentOfType<Args...>(std::forward<Callback>(callback));
}
template<typename ...Components>
inline void SystemContext<Components...>::ForEachEntity(std::function<void(const Entity&)> callback)
{
	world->GetEntityManager().ForEachEntity(callback);
}
template<typename ...Components>
inline void SystemContext<Components...>::ForEachEntity(std::function<void(const Entity&)> callback, LevelID level)
{
	world->GetEntityManager().ForEachEntity(callback, level);
}
template<typename ...Components>
inline void SystemContext<Components...>::ForEachEntity(std::function<void(const Entity&)> callback, const Entity& root)
{
	world->GetEntityManager().ForEachEntity(callback, root);
}

template<typename ...Components>
template<typename T>
inline void SystemContext<Components...>::QueueEvent(const T& event)
{
	commandBuffer->Command_QueueEvent(typeid(T), &event);
}
template<typename ...Components>
template<typename T>
inline const std::vector<T>& SystemContext<Components...>::PollEvents() const
{
	return world->PollEvents<T>();
}

template<typename ...Components>
inline bool SystemContext<Components...>::IsEntityActive(Entity entity)
{
	return world->GetEntityManager().IsEntityActive(entity);
}
template<typename ...Components>
inline std::string SystemContext<Components...>::GetEntityName(Entity entity)
{
	return world->GetEntityManager().GetEntityName(entity);
}
template<typename ...Components>
inline LevelID SystemContext<Components...>::GetEntityLevel(Entity entity)
{
	return world->GetEntityManager().GetEntityLevel(entity);
}
template<typename ...Components>
inline std::filesystem::path SystemContext<Components...>::GetEntityPrefabPath(Entity entity)
{
	return world->GetEntityManager().GetEntityPrefabPath(entity);
}
template<typename ...Components>
inline Entity SystemContext<Components...>::GetEntityParent(Entity entity)
{
	return world->GetEntityManager().GetEntityParent(entity);
}
template<typename ...Components>
inline Entity SystemContext<Components...>::GetEntityFirstChild(Entity entity)
{
	return world->GetEntityManager().GetEntityFirstChild(entity);
}
template<typename ...Components>
inline Entity SystemContext<Components...>::GetEntityNextSibling(Entity entity)
{
	return world->GetEntityManager().GetEntityNextSibling(entity);
}
template<typename ...Components>
inline Entity SystemContext<Components...>::GetEntityLastSibling(Entity entity)
{
	return world->GetEntityManager().GetEntityLastSibling(entity);
}
template<typename ...Components>
inline uint32_t SystemContext<Components...>::GetEntityDepth(Entity entity)
{
	return world->GetEntityManager().GetEntityDepth(entity);
}

template<typename ...Components>
template<typename ...Args>
inline SystemContext<Args...> SystemContext<Components...>::GetContext()
{
	static_assert(((is_one_of_v<Args, Components...>) && ...) && "Component set not compatible with context signature.");

	return ConstructContext<Args...>();
}