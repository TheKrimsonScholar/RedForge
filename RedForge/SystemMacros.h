#pragma once

#include <unordered_map>
#include <typeindex>
#include <string>

#include "Exports.h"
#include "System.h"

struct SystemInfo
{
	std::function<SystemBase*()> createSystem;

	size_t systemID;
	std::string systemName;
	std::vector<std::type_index> dependencies;
	std::vector<std::type_index> dependents;
	std::vector<std::type_index> writeList;
	std::vector<std::type_index> readList;
	size_t phaseIndex = -1;
};

REDFORGE_API std::vector<std::type_index>& GetRegisteredSystemsList();
REDFORGE_API std::unordered_map<std::type_index, SystemInfo>& GetRegisteredSystemInfoMap();

#define GET_SYSTEM_ID(systemTypeID) ((size_t) GetRegisteredSystemInfoMap()[systemTypeID].systemID)
#define GET_SYSTEM_NAME(systemTypeID) ((std::string) GetRegisteredSystemInfoMap()[systemTypeID].systemName)
#define GET_SYSTEM_DEPENDENCIES(systemTypeID) ((std::vector<std::type_index>) GetRegisteredSystemInfoMap()[systemTypeID].dependencies)
#define GET_SYSTEM_DEPENDENTS(systemTypeID) ((std::vector<std::type_index>) GetRegisteredSystemInfoMap()[systemTypeID].dependents)
#define GET_SYSTEM_WRITELIST(systemTypeID) ((std::vector<std::type_index>) GetRegisteredSystemInfoMap()[systemTypeID].writeList)
#define GET_SYSTEM_READLIST(systemTypeID) ((std::vector<std::type_index>) GetRegisteredSystemInfoMap()[systemTypeID].readList)
#define GET_SYSTEM_PHASEINDEX(systemTypeID) ((size_t) GetRegisteredSystemInfoMap()[systemTypeID].phaseIndex)