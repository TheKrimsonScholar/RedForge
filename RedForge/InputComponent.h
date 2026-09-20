#pragma once

#include <unordered_map>
#include <functional>
#include <cstdint>

#include "ComponentRegistration.h"

struct InputComponent : public IComponent
{
	SERIALIZED_COMPONENT_VARS()
	REPLICATED_COMPONENT_VARS()

	std::unordered_map<uint32_t, std::function<void(Entity)>> mouseDownCallbacks;

	std::unordered_map<uint32_t, std::function<void(Entity)>> keyDownCallbacks;
};

REGISTER_COMPONENT_TYPE_BEGIN(InputComponent, IComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VARS_END
REGISTER_COMPONENT_TYPE_END(InputComponent)