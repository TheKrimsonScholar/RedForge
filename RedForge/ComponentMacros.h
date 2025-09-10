#pragma once

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <string>
#include <ostream>
#include <istream>

#include "Exports.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct ComponentVariableInfo
{
	std::type_index variableType;
	std::string variableName;

	std::function<std::istream&(std::istream&, void*)> readFromFile;
	std::function<std::ostream&(std::ostream&, void*)> writeToFile;
};
struct ComponentInfo
{
	ComponentInfo() = default;
	ComponentInfo(uint32_t componentID, std::string componentName, std::vector<std::type_index> dependencies, std::function<std::vector<std::pair<void*, ComponentVariableInfo>>(void*)> getVariables) :
		componentID(componentID), componentName(componentName), dependencies(dependencies), getVariables(getVariables) {};

	std::function<void()> registerComponent;

	uint32_t componentID;
	std::string componentName;
	std::vector<std::type_index> dependencies;
	std::vector<std::type_index> dependents;
	std::function<std::vector<std::pair<void*, ComponentVariableInfo>>(void*)> getVariables;
};

REDFORGE_API std::vector<std::type_index>& GetRegisteredComponentsList();
REDFORGE_API std::unordered_map<std::type_index, ComponentInfo>& GetRegisteredComponentInfoMap();

#define GET_COMPONENT_ID(componentTypeID) ((uint32_t) GetRegisteredComponentInfoMap()[componentTypeID].componentID)
#define GET_COMPONENT_NAME(componentTypeID) ((std::string) GetRegisteredComponentInfoMap()[componentTypeID].componentName)
#define GET_COMPONENT_DEPENDENCIES(componentTypeID) ((std::vector<std::type_index>) GetRegisteredComponentInfoMap()[componentTypeID].dependencies)
#define GET_COMPONENT_DEPENDENTS(componentTypeID) ((std::vector<std::type_index>) GetRegisteredComponentInfoMap()[componentTypeID].dependents)
// Returns a vector of void*-ComponentVariableInfo pairs indicating the location of variables and their type/name data
#define GET_COMPONENT_VARS(componentTypeID, componentPtr) ((std::vector<std::pair<void*, ComponentVariableInfo>>) GetRegisteredComponentInfoMap()[componentTypeID].getVariables(componentPtr))

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
	return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}
inline std::istream& operator>>(std::istream& is, glm::vec3& v)
{
	char filler;
	return is >> filler >> v.x >> filler >> v.y >> filler >> v.z >> filler;
}

inline std::ostream& operator<<(std::ostream& os, const glm::quat& q)
{
	return os << '(' << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ')';
}
inline std::istream& operator>>(std::istream& is, glm::quat& q)
{
	char filler;
	return is >> filler >> q.x >> filler >> q.y >> filler >> q.z >> filler >> q.w >> filler;
}