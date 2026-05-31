#pragma once

#include "System.h"
#include "SystemRegistrationMacros.h"

#include "Window.h"

class WindowSystem : public System<Window>
{
public:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void PostStartup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

	REDFORGE_API void Update(SystemContext<Window>& ctx, float deltaTime) override;

	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
};
REGISTER_SYSTEM(WindowSystem)