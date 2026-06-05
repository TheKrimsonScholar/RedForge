#pragma once

#include "ResourceRegistrationMacros.h"

#include <glm/glm.hpp>

struct DebugCameraState : public IResource
{
	float fov = 60.0f;
	float nearClipPlaneDistance = 0.001f;
	float farClipPlaneDistance = 10000.0f;

	float moveSpeed = 20.0f;
	float lookSpeed = 20.0f;

	bool isActive;

	glm::vec3 location;

	glm::vec2 pitchYaw;

private:
	glm::dvec2 mouseDragStartPosition;
	glm::dvec2 previousMousePosition;
	
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	friend class DebugCameraSystem;

private:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

public:
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetForward() const;
};
REGISTER_RESOURCE(DebugCameraState)
