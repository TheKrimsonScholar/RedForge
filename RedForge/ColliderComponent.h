#pragma once

#include "KMath.h"

#include "TransformComponent.h"

#include "ComponentRegistration.h"

enum class EColliderType
{
    NoCollider,
    Box,
    Sphere
};
struct ColliderComponent : public IComponent
{
    SERIALIZED_COMPONENT_VARS()
	REPLICATED_COMPONENT_VARS()

    EColliderType colliderType;

    glm::vec3 center;
    glm::vec3 halfSize;
    float radius;

public:
    glm::mat4 GetInertiaTensor(float mass) const;
};