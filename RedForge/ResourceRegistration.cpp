#include "ResourceRegistration.h"

void RegisterResourceTypeBase::RegisterResourceDependency(TypeID typeID, TypeID dependency)
{
	GetRegistryInstance().resourceDependencyHierarchy.AddConnection(dependency, typeID);
}
void RegisterResourceTypeBase::RegisterResourceVariables(TypeID typeID, const std::function<std::vector<std::pair<void*, ComponentVariableTypeInfo>>(void*)>& getVariables)
{
	std::unordered_map<TypeID, ComponentTypeInfo>& registeredComponentInfoMap = GetRegistryInstance().registeredComponentInfoMap;

	ComponentTypeInfo& componentInfo = registeredComponentInfoMap.at(typeID);
	componentInfo.getVariables = getVariables;
}