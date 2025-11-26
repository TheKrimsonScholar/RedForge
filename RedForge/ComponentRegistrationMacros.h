#pragma once

#include "ComponentMacros.h"
#include "EntityManager.h"

#define REGISTER_COMPONENT(Type)	\
REGISTER_COMPONENT_BEGIN(Type)		\
REGISTER_COMPONENT_END(Type)
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
		componentInfo.registerComponent = []() { EntityManager::RegisterComponent<Type>(); };															\
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
static inline RegisterComponent_##Type registerComponent_##Type;