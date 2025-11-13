#pragma once

#include <iostream>
#include <format>

#include "DebugManager.h"

#define LOG(Format, ...) DebugManager::PrintLogMessage(LogType::Engine, std::format(Format, __VA_ARGS__));