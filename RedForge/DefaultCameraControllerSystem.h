#pragma once

#include "System.h"

#include "SystemRegistrationMacros.h"

#include "InputSystem.h"

#include "InputState.h"
#include "TimeResource.h"

#include "DefaultCameraControllerComponent.h"
#include "TransformComponent.h"

class DefaultCameraControllerSystem : public System<const InputState, const Time, const DefaultCameraControllerComponent, TransformComponent>
{
public:
	DefaultCameraControllerSystem() {};
	~DefaultCameraControllerSystem() {};

	void Startup(const EngineStartupParams& params, World& world) override;
	void PostStartup(const EngineStartupParams& params, World& world) override;
	void Shutdown(const EngineShutdownParams& params, World& world) override;

	void Update(LocalSystemContext& ctx, float deltaTime) override;
};
REGISTER_SYSTEM_BEGIN(DefaultCameraControllerSystem)
SYSTEM_REQUIRES(InputSystem)
REGISTER_SYSTEM_END(DefaultCameraControllerSystem)