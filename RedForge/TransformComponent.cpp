#include "TransformComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

glm::vec3 TransformComponent::GetRight()
{
	return rotation * glm::vec3(1, 0, 0);
}
glm::vec3 TransformComponent::GetUp()
{
	return rotation * glm::vec3(0, 1, 0);
}
glm::vec3 TransformComponent::GetForward()
{
	return -(rotation * glm::vec3(0, 0, 1));
}

glm::mat4 TransformComponent::GetMatrix()
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), location);
	transform *= glm::mat4_cast(rotation);
	transform = glm::scale(transform, scale);

	return transform;
}