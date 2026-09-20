#pragma once

#include "ResourceRegistration.h"

#include "TransformComponent.h"
#include "CameraComponent.h"

#include <glm/glm.hpp>

struct Cameras : public IResource
{
	SERIALIZED_RESOURCE_VARS()
	REPLICATED_RESOURCE_VARS()

	glm::mat4* viewMatrixOverride = nullptr;
	glm::mat4* projectionMatrixOverride = nullptr;

	Entity mainCameraEntity = {};

protected:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

public:
	glm::mat4 GetMainViewMatrix(SystemContext<const TransformComponent> ctx) const;
	glm::mat4 GetMainProjectionMatrix(SystemContext<const CameraComponent> ctx) const;
};
REGISTER_RESOURCE_TYPE(Cameras, IResource)