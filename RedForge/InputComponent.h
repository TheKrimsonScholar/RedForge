#pragma once

#include <unordered_map>
#include <functional>
#include <cstdint>

#include "EntityManager.h"

struct InputComponent
{
	std::unordered_map<uint32_t, std::function<void(Entity)>> mouseDownCallbacks;

	std::unordered_map<uint32_t, std::function<void(Entity)>> keyDownCallbacks;
};