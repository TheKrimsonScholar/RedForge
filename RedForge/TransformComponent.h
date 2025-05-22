#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent
{
	glm::vec3 location;
	glm::quat rotation;
	glm::vec3 scale;

public:
	glm::mat4 GetMatrix();
};