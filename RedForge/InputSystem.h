#pragma once

#include "System.h"

#include "WindowSystem.h"

#include "InputLayer.h"

#include "Exports.h"

class InputSystem : public System<InputState>
{
private:
	static const uint32_t NUM_BUTTONS = 512;

private:
	static inline InputSystem* Instance;

	bool buttonsDown[NUM_BUTTONS] = {};
	bool buttonsPressedThisFrame[NUM_BUTTONS] = {};

public:
	InputSystem() {};
	~InputSystem() {};

	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void PostStartup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;
	
	REDFORGE_API void Update(LocalSystemContext& ctx, float deltaTime) override;
};
REGISTER_SYSTEM_BEGIN(InputSystem)
SYSTEM_REQUIRES(WindowSystem)
REGISTER_SYSTEM_END(InputSystem)