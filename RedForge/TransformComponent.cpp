#include "TransformComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

glm::mat4 TransformComponent::GetMatrix()
{
	return glm::translate(glm::mat4(1.0f), location) * glm::scale(glm::toMat4(rotation), scale);
}