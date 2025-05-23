#pragma once

#include <glm/glm.hpp>

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