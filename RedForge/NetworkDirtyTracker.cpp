#include "NetworkDirtyTracker.h"

std::unordered_set<TypeID> NetworkDirtyTracker::dirtyResources;
std::unordered_set<ComponentRef> NetworkDirtyTracker::dirtyComponents;

void NetworkDirtyTracker::MarkResourceDirty(TypeID resourceType)
{
    dirtyResources.insert(resourceType);
}
void NetworkDirtyTracker::MarkComponentDirty(const Entity& entity, TypeID componentType)
{
    dirtyComponents.emplace(entity, componentType);
}

std::unordered_set<TypeID> NetworkDirtyTracker::GetDirtyResources()
{
    return dirtyResources;
}
std::unordered_set<ComponentRef> NetworkDirtyTracker::GetDirtyComponents()
{
    return dirtyComponents;
}

void NetworkDirtyTracker::ClearDirtyResources()
{
    dirtyResources.clear();
}
void NetworkDirtyTracker::ClearDirtyComponents()
{
    dirtyComponents.clear();
}