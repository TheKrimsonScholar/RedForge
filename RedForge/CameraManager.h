#pragma once

#include "System.h"
#include "SystemRegistration.h"

#include "WindowSystem.h"

#include "Cameras.h"
#include "Window.h"

#include "CameraComponent.h"

class CameraManager : public System<const Cameras, const Window, CameraComponent>
{
private:
	static inline CameraManager* Instance;

public:
	CameraManager() {};
	~CameraManager() {};

	void Startup(const EngineStartupParams& params, World& world) override;
	void PostStartup(const EngineStartupParams& params, World& world) override;
	void Shutdown(const EngineShutdownParams& params, World& world) override;

	void Update(SystemContext<const Cameras, const Window, CameraComponent>& ctx, float deltaTime) override;
};
REGISTER_SYSTEM_TYPE_BEGIN(CameraManager, System<const Cameras, const Window, CameraComponent>)
SYSTEM_REQUIRES(WindowSystem)
REGISTER_SYSTEM_TYPE_END(CameraManager)