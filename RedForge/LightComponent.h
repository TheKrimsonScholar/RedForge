#pragma once

#include <cstdint>

#include "KMath.h"

#include "ComponentRegistration.h"

enum class ELightType : uint32_t
{
	Directional,
	Point,
	Spot
};
struct LightComponent : public IComponent
{
    SERIALIZED_COMPONENT_VARS()
	REPLICATED_COMPONENT_VARS()

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