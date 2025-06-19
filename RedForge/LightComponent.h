#pragma once

#include <cstdint>

#include <glm/glm.hpp>

#include "ComponentMacros.h"

enum class ELightType : uint32_t
{
	Directional,
	Point,
	Spot
};
struct LightComponent
{
    ELightType lightType;
    
    /* All types */
    
    glm::vec3 color;
    float intensity;

    /* Directional */

    glm::vec3 direction;
    
    /* Point, Spot */

    glm::vec3 location;
    float range;
    
    /* Spot */
    
    float spotInnerAngle;
    float spotOuterAngle;
};

REGISTER_COMPONENT_BEGIN(LightComponent)
//COMPONENT_VAR(ELightType, lightType)
COMPONENT_VAR(glm::vec3, color)
COMPONENT_VAR(float, intensity)
COMPONENT_VAR(glm::vec3, direction)
COMPONENT_VAR(glm::vec3, location)
COMPONENT_VAR(float, range)
COMPONENT_VAR(float, spotInnerAngle)
COMPONENT_VAR(float, spotOuterAngle)
REGISTER_COMPONENT_END(LightComponent)