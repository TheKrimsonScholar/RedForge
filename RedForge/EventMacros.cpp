#include "EventMacros.h"

std::vector<std::type_index>& GetRegisteredEventsList()
{
	static std::vector<std::type_index> list;

	return list;
}
std::unordered_map<std::type_index, EventInfo>& GetRegisteredEventInfoMap()
{
	static std::unordered_map<std::type_index, EventInfo> map;

	return map;
}