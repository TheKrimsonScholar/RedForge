#include "DebugLogEvent.h"

REGISTER_EVENT_TYPE_BEGIN(DebugLogEvent, IEvent)
EVENT_VARS_BEGIN
	EVENT_VAR(ELogType, logType)
	EVENT_VAR(std::string, text)
EVENT_VARS_END
REGISTER_EVENT_TYPE_END(DebugLogEvent)

DebugLogEvent::DebugLogEvent(ELogType logType, std::string text)
	: logType(logType), text(text)
{

}
DebugLogEvent::DebugLogEvent(const DebugLogEvent& other)
	: logType(other.logType), text(other.text)
{

}
DebugLogEvent::~DebugLogEvent()
{
	int k = 0;
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

//template<>
//void ByteStreamBase::Serialize(const DebugLogEvent& src)
//{
//	*this << src.logType;
//	*this << src.text;
//}
//template<>
//void ByteStreamBase::Deserialize(DebugLogEvent& dst)
//{
//	*this >> dst.logType;
//	*this >> dst.text;
//}