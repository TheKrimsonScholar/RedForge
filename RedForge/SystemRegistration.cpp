#include "SystemRegistration.h"

void RegisterSystemTypeBase::RegisterSystemDependency(TypeID typeID, TypeID dependency)
{
    GetRegistryInstance().systemDependencyHierarchy.AddConnection(dependency, typeID);
}