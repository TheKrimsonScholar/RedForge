#include "SystemMacros.h"

std::vector<std::type_index>& GetRegisteredSystemsList()
{
	static std::vector<std::type_index> list;

	return list;
}
std::unordered_map<std::type_index, SystemInfo>& GetRegisteredSystemInfoMap()
{
	static std::unordered_map<std::type_index, SystemInfo> map;

	return map;
}