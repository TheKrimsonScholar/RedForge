#pragma once

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <string>

#include "Exports.h"

struct ComponentVariableInfo
{
	std::type_index variableType;
	std::string variableName;
};
struct ComponentInfo
{
	ComponentInfo() = default;
	ComponentInfo(std::string componentName, std::function<std::vector<std::pair<void*, ComponentVariableInfo>>(void*)> getVariables) : 
		componentName(componentName), getVariables(getVariables) {};

	std::string componentName;
	std::function<std::vector<std::pair<void*, ComponentVariableInfo>>(void*)> getVariables;
};

REDFORGE_API std::vector<std::type_index>& GetRegisteredComponentsList();
REDFORGE_API std::unordered_map<std::type_index, ComponentInfo>& GetRegisteredComponentInfoMap();

#define GET_COMPONENT_NAME(componentTypeID) GetRegisteredComponentInfoMap()[componentTypeID].componentName
#define GET_COMPONENT_VARS(componentTypeID, componentPtr) GetRegisteredComponentInfoMap()[componentTypeID].getVariables(componentPtr)

#define REGISTER_COMPONENT_BEGIN(Type)																													\
struct RegisterComponent_##Type																															\
{																																						\
	RegisterComponent_##Type()																															\
	{																																					\
		if(std::find(GetRegisteredComponentsList().begin(), GetRegisteredComponentsList().end(), typeid(Type)) != GetRegisteredComponentsList().end())	\
			return;																																		\
		if(GetRegisteredComponentInfoMap().find(typeid(Type)) != GetRegisteredComponentInfoMap().end())													\
			return;																																		\
																																						\
		GetRegisteredComponentsList().push_back(typeid(Type));																							\
		GetRegisteredComponentInfoMap().emplace(typeid(Type), ComponentInfo(																			\
			#Type,																																		\
			[](void* rawComponent) -> std::vector<std::pair<void*, ComponentVariableInfo>>																\
				{																																		\
					Type* component = static_cast<Type*>(rawComponent);																				\
																																						\
					return																																\
					{
#define COMPONENT_VAR(variableType, variableName)																										\
						{ &component->variableName, { typeid(variableType), #variableName }},
#define REGISTER_COMPONENT_END(Type)																													\
					};																																	\
				} ));																																	\
	}																																					\
};																																						\
static RegisterComponent_##Type registerComponent_##Type;