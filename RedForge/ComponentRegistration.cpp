#include "ComponentRegistration.h"

void RegisterComponentTypeBase::RegisterComponentDependency(TypeID typeID, TypeID dependency)
{
	GetRegistryInstance().componentDependencyHierarchy.AddConnection(dependency, typeID);
}
void RegisterComponentTypeBase::RegisterComponentVariables(TypeID typeID, const std::function<std::vector<std::pair<void*, ComponentVariableTypeInfo>>(void*)>& getVariables)
{
	std::unordered_map<TypeID, ComponentTypeInfo>& registeredComponentInfoMap = GetRegistryInstance().registeredComponentInfoMap;

	ComponentTypeInfo& componentInfo = registeredComponentInfoMap.at(typeID);
	componentInfo.getVariables = getVariables;
}