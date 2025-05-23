#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent
{
	glm::vec3 location;
	glm::quat rotation;
	glm::vec3 scale;

public:
	glm::vec3 GetRight();
	glm::vec3 GetUp();
	glm::vec3 GetForward();

	glm::mat4 GetMatrix();
};