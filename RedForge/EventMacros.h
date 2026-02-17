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

struct IEvent {};

class IEventQueue;

struct EventVariableInfo
{
	std::type_index variableType;
	std::string variableName;
};
struct EventInfo
{
	EventInfo() = default;
	EventInfo(uint32_t eventID, std::string eventName, std::function<std::vector<std::pair<void*, EventVariableInfo>>(void*)> getVariables) :
		eventID(eventID), eventName(eventName), getVariables(getVariables) {};

	std::function<IEventQueue*()> createQueue;

	uint32_t eventID;
	std::string eventName;
	size_t eventSize;
	std::function<std::vector<std::pair<void*, EventVariableInfo>>(void*)> getVariables;
};

REDFORGE_API std::vector<std::type_index>& GetRegisteredEventsList();
REDFORGE_API std::unordered_map<std::type_index, EventInfo>& GetRegisteredEventInfoMap();

#define GET_EVENT_ID(eventTypeID) ((uint32_t) GetRegisteredEventInfoMap()[eventTypeID].eventID)
#define GET_EVENT_NAME(eventTypeID) ((std::string) GetRegisteredEventInfoMap()[eventTypeID].eventName)
#define GET_EVENT_SIZE(eventTypeID) ((size_t) GetRegisteredEventInfoMap()[eventTypeID].eventSize)
// Returns a vector of void*-EventVariableInfo pairs indicating the location of variables and their type/name data
#define GET_EVENT_VARS(eventTypeID, eventPtr) ((std::vector<std::pair<void*, EventVariableInfo>>) GetRegisteredEventInfoMap()[eventTypeID].getVariables(eventPtr))