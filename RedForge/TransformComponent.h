#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent
{
	glm::vec3 location;
	glm::quat rotation;
	glm::vec3 scale;

public:
	void MoveRelative(glm::vec3 vector);

	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetForward() const;

	glm::mat4 GetMatrix() const;
	glm::mat4 GetRotationMatrix() const;

	glm::vec3 LocalToWorld_Point(const glm::vec3& vector, bool includeScale = false) const;
	glm::vec3 LocalToWorld_Direction(const glm::vec3& vector, bool includeScale = false) const;
};