#pragma once

#include "System.h"

#include "WindowSystem.h"

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
REGISTER_SYSTEM_BEGIN(DebugCameraSystem)
SYSTEM_REQUIRES(WindowSystem)
REGISTER_SYSTEM_END(DebugCameraSystem)