#pragma once

#include "ComponentRegistration.h"

#include "TransformComponent.h"

struct DefaultCameraControllerComponent : public IComponent
{
	SERIALIZED_COMPONENT_VARS()
	REPLICATED_COMPONENT_VARS()
};
REGISTER_COMPONENT_TYPE_BEGIN(DefaultCameraControllerComponent, IComponent)
COMPONENT_REQUIRES(TransformComponent)
REGISTER_COMPONENT_TYPE_END(DefaultCameraControllerComponent)