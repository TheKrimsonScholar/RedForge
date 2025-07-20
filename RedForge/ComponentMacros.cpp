#include "ComponentMacros.h"

std::vector<std::type_index>& GetRegisteredComponentsList()
{
	static std::vector<std::type_index> list;

	return list;
}
std::unordered_map<std::string, std::type_index>& GetRegisteredComponentNameMap()
{
	static std::unordered_map<std::string, std::type_index> map;

	return map;
}
std::unordered_map<std::type_index, ComponentInfo>& GetRegisteredComponentInfoMap()
{
	static std::unordered_map<std::type_index, ComponentInfo> map;

	return map;
}