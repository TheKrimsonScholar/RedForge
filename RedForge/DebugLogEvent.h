#pragma once

#include "EventRegistrationMacros.h"

enum class ELogType
{
	Cout,
	Engine,
	Editor,
	Game
};
struct DebugLogEvent : IEvent
{
	ELogType logType = ELogType::Cout;
	std::string text = "";

	DebugLogEvent(ELogType logType = ELogType::Cout, std::string text = "");

	REDFORGE_API std::string ToString() const;
};