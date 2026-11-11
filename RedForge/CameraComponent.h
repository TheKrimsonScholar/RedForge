#pragma once

#include <glm/glm.hpp>

#include "ComponentRegistration.h"

#include "SystemContext.h"
#include "TransformComponent.h"

enum class ECameraProjectionType
{
	Perspective,
	Orthographic
};

struct CameraComponent : public IComponent
{
	SERIALIZED_COMPONENT_VARS()
	REPLICATED_COMPONENT_VARS()

	ECameraProjectionType projectionType;

	float nearClipPlaneDistance;
	float farClipPlaneDistance;

	/* Perspective */
	
	float fov;
	float aspectRatio;

	/* Orthographic */

	float left;
	float right;
	float bottom;
	float top;

public:
	glm::mat4 GetProjectionMatrix() const;
};

glm::mat4 GetViewMatrix(Entity cameraEntity, SystemContext<const TransformComponent> ctx);