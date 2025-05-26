#include "TransformComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

void TransformComponent::MoveRelative(glm::vec3 vector)
{
	location += rotation * vector;
}

glm::vec3 TransformComponent::GetRight() const
{
	return rotation * glm::vec3(1, 0, 0);
}
glm::vec3 TransformComponent::GetUp() const
{
	return rotation * glm::vec3(0, 1, 0);
}
glm::vec3 TransformComponent::GetForward() const
{
	return -(rotation * glm::vec3(0, 0, 1));
}

glm::mat4 TransformComponent::GetMatrix() const
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), location);
	transform *= glm::mat4_cast(rotation);
	transform = glm::scale(transform, scale);

	return transform;
}
glm::mat4 TransformComponent::GetRotationMatrix() const
{
	return glm::transpose(glm::mat4(
		glm::vec4(GetRight(), 1),
		glm::vec4(GetUp(), 1),
		glm::vec4(GetForward(), 1),
		glm::vec4(0, 0, 0, 1)));
}

glm::vec3 TransformComponent::LocalToWorld_Point(const glm::vec3& vector, bool includeScale) const
{
	return location + GetRight() * vector.x + GetUp() * vector.y + GetForward() * vector.z;
}
glm::vec3 TransformComponent::LocalToWorld_Direction(const glm::vec3& vector, bool includeScale) const
{
	return GetRight() * vector.x + GetUp() * vector.y + GetForward() * vector.z;
}