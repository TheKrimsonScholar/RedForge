#pragma once

#include <glm/glm.hpp>

enum class EColliderType
{
    None,
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