#include "EventRegistration.h"

void RegisterEventTypeBase::RegisterEventVariables(TypeID typeID, const std::function<std::vector<std::pair<void*, EventVariableTypeInfo>>(void*)>& getVariables)
{
	std::unordered_map<TypeID, EventTypeInfo>& registeredEventInfoMap = GetRegistryInstance().registeredEventInfoMap;

	EventTypeInfo& eventInfo = registeredEventInfoMap.at(typeID);
	eventInfo.getVariables = getVariables;
}