#pragma once

#include <cstdint>

#include <glm/glm.hpp>

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