#pragma once

#include "SystemMacros.h"

#define REGISTER_SYSTEM(Type)	\
REGISTER_SYSTEM_BEGIN(Type)		\
REGISTER_SYSTEM_END(Type)
#define REGISTER_SYSTEM_BEGIN(Type)																													\
struct RegisterSystem_##Type																															\
{																																						\
	RegisterSystem_##Type()																															\
	{																																					\
		std::type_index typeID = typeid(Type);																											\
																																						\
		if(std::find(GetRegisteredSystemsList().begin(), GetRegisteredSystemsList().end(), typeID) != GetRegisteredSystemsList().end())		\
			return;																																		\
		if(GetRegisteredSystemInfoMap().find(typeID) != GetRegisteredSystemInfoMap().end())														\
			return;																																		\
																																						\
		GetRegisteredSystemsList().push_back(typeID);																								\
																																						\
		SystemInfo& systemInfo = GetRegisteredSystemInfoMap()[typeID];																			\
		systemInfo.systemID = GetRegisteredSystemInfoMap().size();																				\
		systemInfo.systemName = #Type;
#define SYSTEM_REQUIRES(systemType)																												\
		systemInfo.dependencies.push_back(typeid(systemType));																					\
		GetRegisteredSystemInfoMap()[typeid(systemType)].dependents.push_back(typeID);
#define REGISTER_SYSTEM_END(Type)																													\
		Type::GetComponentLists(systemInfo.writeList, systemInfo.readList);																				\
		systemInfo.instance = new Type();																												\
	}																																					\
};																																						\
static inline RegisterSystem_##Type registerSystem_##Type;