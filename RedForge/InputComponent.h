#pragma once

#include <unordered_map>
#include <functional>
#include <cstdint>

#include "ComponentRegistrationMacros.h"

struct InputComponent
{
	std::unordered_map<uint32_t, std::function<void(Entity)>> mouseDownCallbacks;

	std::unordered_map<uint32_t, std::function<void(Entity)>> keyDownCallbacks;
};

REGISTER_COMPONENT_BEGIN(InputComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VARS_END
REGISTER_COMPONENT_END(InputComponent)