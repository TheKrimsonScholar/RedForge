#pragma once

#include "ResourceRegistrationMacros.h"

#include "TransformComponent.h"
#include "CameraComponent.h"

#include <glm/glm.hpp>

struct Cameras : public IResource
{
	glm::mat4* viewMatrixOverride = nullptr;
	glm::mat4* projectionMatrixOverride = nullptr;

	Entity mainCameraEntity = {};

private:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

public:
	glm::mat4 GetMainViewMatrix(SystemContext<const TransformComponent> ctx) const;
	glm::mat4 GetMainProjectionMatrix(SystemContext<const CameraComponent> ctx) const;
};
REGISTER_RESOURCE(Cameras)