#pragma once

#include "EntityManager.h"
#include "ComponentRef.h"

class NetworkDirtyTracker
{
private:
	static std::unordered_set<TypeID> dirtyResources;
	static std::unordered_set<ComponentRef> dirtyComponents;

public:
	template<typename T>
	static void MarkResourceDirty();
	REDFORGE_API static void MarkResourceDirty(TypeID resourceType);
	template<typename T>
	static void MarkComponentDirty(const Entity& entity);
	REDFORGE_API static void MarkComponentDirty(const Entity& entity, TypeID componentType);

	static std::unordered_set<TypeID> GetDirtyResources();
	static std::unordered_set<ComponentRef> GetDirtyComponents();

private:
	static void ClearDirtyResources();
	static void ClearDirtyComponents();

	friend class NetworkSystem;
};

template<typename T>
inline void NetworkDirtyTracker::MarkResourceDirty()
{
	MarkResourceDirty(TYPEID(T));
}
template<typename T>
inline void NetworkDirtyTracker::MarkComponentDirty(const Entity& entity)
{
	MarkComponentDirty(entity, TYPEID(T));
}