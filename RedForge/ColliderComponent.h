#pragma once

#include <glm/glm.hpp>

#include "ComponentMacros.h"

enum class EColliderType
{
    NoCollider,
    Box,
    Sphere
};
struct ColliderComponent
{
    EColliderType colliderType;

    glm::vec3 center;
    glm::vec3 halfSize;
    float radius;

public:
    glm::mat4 GetInertiaTensor(float mass) const;
};

REGISTER_COMPONENT_BEGIN(ColliderComponent)
COMPONENT_VARS_BEGIN
//COMPONENT_VAR(EColliderType, colliderType)
COMPONENT_VAR(glm::vec3, center)
COMPONENT_VAR(glm::vec3, halfSize)
COMPONENT_VAR(float, radius)
COMPONENT_VARS_END
REGISTER_COMPONENT_END(ColliderComponent)