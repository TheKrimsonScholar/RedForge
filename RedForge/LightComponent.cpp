#include "LightComponent.h"

REGISTER_COMPONENT_TYPE_BEGIN(LightComponent, IComponent)
COMPONENT_VARS_BEGIN
//COMPONENT_VAR(ELightType, lightType)
COMPONENT_VAR(glm::vec3, color)
COMPONENT_VAR(float, intensity)
COMPONENT_VAR(glm::vec3, direction)
COMPONENT_VAR(glm::vec3, location)
COMPONENT_VAR(float, range)
COMPONENT_VAR(float, spotInnerAngle)
COMPONENT_VAR(float, spotOuterAngle)
COMPONENT_VARS_END
REGISTER_COMPONENT_TYPE_END(LightComponent)