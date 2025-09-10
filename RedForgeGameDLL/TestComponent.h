#pragma once

#include "RedForgeGame.h"

#include "ComponentRegistrationMacros.h"

REDFORGEGAME_API struct TestComponent
{
	float k;
};

REGISTER_COMPONENT_BEGIN(TestComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VAR(float, k)
COMPONENT_VARS_END
REGISTER_COMPONENT_END(TestComponent)