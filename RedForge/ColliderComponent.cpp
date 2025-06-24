#include "ColliderComponent.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 ColliderComponent::GetInertiaTensor(float mass) const
{
	switch(colliderType)
	{
	case EColliderType::NoCollider: break;
	case EColliderType::Box:
	{
		return glm::scale(glm::mat4(1.0f), glm::vec3(
			(1.0f / 12) * mass * (pow(2 * halfSize.x, 2) + pow(2 * halfSize.y, 2)),
			(1.0f / 12) * mass * (pow(2 * halfSize.x, 2) + pow(2 * halfSize.z, 2)),
			(1.0f / 12) * mass * (pow(2 * halfSize.y, 2) + pow(2 * halfSize.z, 2))));
	}
	break;
	case EColliderType::Sphere:
	{
		return glm::mat4(1.0f) * ((2.0f / 3) * mass * radius * radius);
	}
	break;
	default: break;
	}
}