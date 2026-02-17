#include "ResourceMacros.h"

std::vector<std::type_index>& GetRegisteredResourcesList()
{
	static std::vector<std::type_index> list;

	return list;
}
std::unordered_map<std::type_index, ResourceInfo>& GetRegisteredResourceInfoMap()
{
	static std::unordered_map<std::type_index, ResourceInfo> map;

	return map;
}