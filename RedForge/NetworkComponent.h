#pragma once

#include "ComponentRegistration.h"

struct NetworkComponent : public IComponent
{
	SERIALIZED_COMPONENT_VARS()
	REPLICATED_COMPONENT_VARS()

	//uint32_t networkID = -1;
};

REGISTER_COMPONENT_TYPE_BEGIN(NetworkComponent, IComponent)
COMPONENT_VARS_BEGIN
//COMPONENT_VAR(uint32_t, networkID)
COMPONENT_VARS_END
REGISTER_COMPONENT_TYPE_END(NetworkComponent)