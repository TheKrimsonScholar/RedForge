#include "DebugLogEvent.h"

REGISTER_EVENT_BEGIN(DebugLogEvent)
EVENT_VARS_BEGIN
		EVENT_VAR(ELogType, logType)
		EVENT_VAR(std::string, text)
EVENT_VARS_END
REGISTER_EVENT_END(DebugLogEvent)

DebugLogEvent::DebugLogEvent(ELogType logType, std::string text)
	: logType(logType), text(text)
{

}

std::string DebugLogEvent::ToString() const
{
	std::string logTypeString = "UNKNOWN";
	switch (logType)
	{
	case ELogType::Cout:
		logTypeString = "COUT";
		break;
	case ELogType::Engine:
		logTypeString = "ENGINE";
		break;
	case ELogType::Editor:
		logTypeString = "EDITOR";
		break;
	case ELogType::Game:
		logTypeString = "GAME";
		break;
	default: break;
	}

	return std::format("[{}] {}", logTypeString, text);
}