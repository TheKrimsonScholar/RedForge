#pragma once

#include "ComponentRegistrationMacros.h"

#include "TransformComponent.h"

class DefaultCameraControllerComponent
{

};
REGISTER_COMPONENT_BEGIN(DefaultCameraControllerComponent)
COMPONENT_REQUIRES(TransformComponent)
REGISTER_COMPONENT_END(DefaultCameraControllerComponent)