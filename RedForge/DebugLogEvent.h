#pragma once

#include "EventRegistration.h"

enum class ELogType : uint8_t
{
	Cout,
	Engine,
	Editor,
	Game
};
struct DebugLogEvent : IEvent
{
	SERIALIZED_EVENT_VARS(logType, text)

	ELogType logType = ELogType::Cout;
	std::string text = "";

	REDFORGE_API DebugLogEvent(ELogType logType = ELogType::Cout, std::string text = "");
	REDFORGE_API DebugLogEvent(const DebugLogEvent& other);
	REDFORGE_API ~DebugLogEvent();

	REDFORGE_API std::string ToString() const;
};

template<>
void ByteStreamBase::Serialize(const DebugLogEvent& src);
template<>
void ByteStreamBase::Deserialize(DebugLogEvent& dst);