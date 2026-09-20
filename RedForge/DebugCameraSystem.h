#pragma once

#include "System.h"
#include "SystemRegistration.h"

#include "WindowSystem.h"
#include "InputSystem.h"

#include "DebugCameraState.h"
#include "Cameras.h"
#include "InputState.h"
#include "Window.h"

class DebugCameraSystem : public System<DebugCameraState, Cameras, const InputState, const Window>
{
	void Startup(const EngineStartupParams& params, World& world) override;
	void PostStartup(const EngineStartupParams& params, World& world) override;
	void Shutdown(const EngineShutdownParams& params, World& world) override;

	void Update(SystemContext<DebugCameraState, Cameras, const InputState, const Window>& ctx, float deltaTime) override;
};
REGISTER_SYSTEM_TYPE_BEGIN(DebugCameraSystem, System<DebugCameraState, Cameras, const InputState, const Window>)
SYSTEM_REQUIRES(WindowSystem)
SYSTEM_REQUIRES(InputSystem)
REGISTER_SYSTEM_TYPE_END(DebugCameraSystem)