#pragma once

#include "ComponentRegistration.h"

#include "Exports.h"

#include "KMath.h"

REDFORGE_API struct TransformComponent : public IReplicatedComponent
{
	SERIALIZED_COMPONENT_VARS(location, rotation, scale)
	REPLICATED_COMPONENT_VARS(location, rotation, scale)

	glm::vec3 location = { 0, 0, 0 };
	glm::quat rotation = { 0, 0, 0, 1 };
	glm::vec3 scale = { 1, 1, 1 };

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