#pragma once

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <string>
#include <ostream>
#include <istream>

#include "EngineParams.h"

#include "Exports.h"

class World;

struct IResource
{
protected:
	virtual void Startup(const EngineStartupParams& params, World& world) = 0;
	virtual void Shutdown(const EngineShutdownParams& params, World& world) = 0;

	friend World;
};

struct ResourceVariableInfo
{
	std::type_index variableType;
	std::string variableName;

	std::function<std::istream&(std::istream&, void*)> readFromFile;
	std::function<std::ostream&(std::ostream&, void*)> writeToFile;
};
struct ResourceInfo
{
	ResourceInfo() = default;
	ResourceInfo(uint32_t resourceID, std::string resourceName, std::function<std::vector<std::pair<void*, ResourceVariableInfo>>(void*)> getVariables) :
		resourceID(resourceID), resourceName(resourceName), getVariables(getVariables) {};

	std::function<IResource*()> createResource;
	
	uint32_t resourceID;
	std::string resourceName;
	std::vector<std::type_index> dependencies;
	std::vector<std::type_index> dependents;
	std::function<std::vector<std::pair<void*, ResourceVariableInfo>>(void*)> getVariables;
};

REDFORGE_API std::vector<std::type_index>& GetRegisteredResourcesList();
REDFORGE_API std::unordered_map<std::type_index, ResourceInfo>& GetRegisteredResourceInfoMap();

#define GET_RESOURCE_ID(resourceTypeID) ((uint32_t) GetRegisteredResourceInfoMap()[resourceTypeID].resourceID)
#define GET_RESOURCE_NAME(resourceTypeID) ((std::string) GetRegisteredResourceInfoMap()[resourceTypeID].resourceName)
#define GET_RESOURCE_DEPENDENCIES(resourceTypeID) ((std::vector<std::type_index>) GetRegisteredResourceInfoMap()[resourceTypeID].dependencies)
#define GET_RESOURCE_DEPENDENTS(resourceTypeID) ((std::vector<std::type_index>) GetRegisteredResourceInfoMap()[resourceTypeID].dependents)
// Returns a vector of void*-ResourceVariableInfo pairs indicating the location of variables and their type/name data
#define GET_RESOURCE_VARS(resourceTypeID, resourcePtr) ((std::vector<std::pair<void*, ResourceVariableInfo>>) GetRegisteredResourceInfoMap()[resourceTypeID].getVariables(resourcePtr))