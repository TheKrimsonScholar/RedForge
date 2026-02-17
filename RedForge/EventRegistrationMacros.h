#pragma once

#include "EventMacros.h"

#include "EventQueue.h"

#define REGISTER_EVENT(Type)	\
REGISTER_EVENT_BEGIN(Type)		\
REGISTER_EVENT_END(Type)
#define REGISTER_EVENT_BEGIN(Type)																													\
struct RegisterEvent_##Type																															\
{																																						\
	RegisterEvent_##Type()																															\
	{																																					\
		std::type_index typeID = typeid(Type);																											\
																																						\
		if(std::find(GetRegisteredEventsList().begin(), GetRegisteredEventsList().end(), typeID) != GetRegisteredEventsList().end())		\
			return;																																		\
		if(GetRegisteredEventInfoMap().find(typeID) != GetRegisteredEventInfoMap().end())														\
			return;																																		\
																																						\
		auto castEventPtr = [](void* rawEvent) -> Type*																							\
			{																																			\
				return static_cast<Type*>(rawEvent);																								\
			};																																			\
																																						\
		GetRegisteredEventsList().push_back(typeID);																								\
																																						\
		EventInfo& eventInfo = GetRegisteredEventInfoMap()[typeID];																			\
		eventInfo.createQueue = []() { return new EventQueue<Type>(); };																				\
		eventInfo.eventID = GetRegisteredEventInfoMap().size();																				\
		eventInfo.eventName = #Type;																										\
		eventInfo.eventSize = sizeof(Type);
#define EVENT_VARS_BEGIN																															\
		eventInfo.getVariables = [castEventPtr](void* rawEvent) -> std::vector<std::pair<void*, EventVariableInfo>>						\
			{																																			\
				auto event = castEventPtr(rawEvent);																						\
																																						\
				return																																	\
				{
#define EVENT_VAR(variableType, variableName)																										\
					{																																	\
						&event->variableName,																										\
						{ typeid(variableType), #variableName }																																\
					},
#define EVENT_VARS_END																																\
				};																																		\
			};
#define REGISTER_EVENT_END(Type)																													\
	}																																					\
};																																						\
static inline RegisterEvent_##Type registerEvent_##Type;