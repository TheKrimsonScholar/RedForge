#pragma once

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <string>

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

#define REGISTER_COMPONENT_BEGIN(Type)																													\
struct RegisterComponent_##Type																															\
{																																						\
	RegisterComponent_##Type()																															\
	{																																					\
		std::type_index typeID = typeid(Type);																											\
																																						\
		if(std::find(GetRegisteredComponentsList().begin(), GetRegisteredComponentsList().end(), typeID) != GetRegisteredComponentsList().end())		\
			return;																																		\
		if(GetRegisteredComponentInfoMap().find(typeID) != GetRegisteredComponentInfoMap().end())														\
			return;																																		\
																																						\
		auto castComponentPtr = [](void* rawComponent) -> Type*																							\
			{																																			\
				return static_cast<Type*>(rawComponent);																								\
			};																																			\
																																						\
		GetRegisteredComponentsList().push_back(typeID);																								\
																																						\
		ComponentInfo& componentInfo = GetRegisteredComponentInfoMap()[typeID];																			\
		componentInfo.componentID = GetRegisteredComponentInfoMap().size();																				\
		componentInfo.componentName = #Type;
#define COMPONENT_REQUIRES(componentType)																												\
		componentInfo.dependencies.push_back(typeid(componentType));																					\
		GetRegisteredComponentInfoMap()[typeid(componentType)].dependents.push_back(typeID);
#define COMPONENT_VARS_BEGIN																															\
		componentInfo.getVariables = [castComponentPtr](void* rawComponent) -> std::vector<std::pair<void*, ComponentVariableInfo>>						\
			{																																			\
				auto component = castComponentPtr(rawComponent);																						\
																																						\
				return																																	\
				{
#define COMPONENT_VAR(variableType, variableName)																										\
					{																																	\
						&component->variableName,																										\
						{																																\
							typeid(variableType), #variableName,																						\
																																						\
							[](std::istream& is, void* variablePtr) -> std::istream&																	\
								{																														\
									return is >> *static_cast<variableType*>(variablePtr);																\
								},																														\
							[](std::ostream& os, void* variablePtr) -> std::ostream&																	\
								{																														\
									return os << *static_cast<variableType*>(variablePtr) << " ";														\
								}																														\
						}																																\
					},
#define COMPONENT_VARS_END																																\
				};																																		\
			};
#define REGISTER_COMPONENT_END(Type)																													\
	}																																					\
};																																						\
static RegisterComponent_##Type registerComponent_##Type;