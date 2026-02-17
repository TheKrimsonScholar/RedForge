#pragma once

#include <iostream>
#include <format>

#include "DebugLogEvent.h"

//#define LOG(Format, ...) DebugManager::PrintLogMessage(LogType::Engine, std::format(Format, __VA_ARGS__));
#define LOG(Context, Format, ...) Context.QueueEvent(DebugLogEvent(ELogType::Engine, std::format(Format, __VA_ARGS__)));