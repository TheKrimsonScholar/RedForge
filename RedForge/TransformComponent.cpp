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
	return glm::translate(glm::mat4(1.0f), location) * glm::scale(glm::toMat4(rotation), scale);
}