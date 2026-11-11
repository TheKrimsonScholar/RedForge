#include "SystemRegistration.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

void RegisterSystemTypeBase::RegisterSystemDependency(TypeID typeID, TypeID dependency)
{
    GetRegistryInstance().systemDependencyHierarchy.AddConnection(dependency, typeID);
}