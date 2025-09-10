#pragma once

#include <glm/glm.hpp>

#include "ComponentRegistrationMacros.h"

enum class ECameraProjectionType
{
	Perspective,
	Orthographic
};

struct CameraComponent
{
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

REGISTER_COMPONENT_BEGIN(CameraComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VARS_END
REGISTER_COMPONENT_END(CameraComponent)