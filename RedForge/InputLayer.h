#pragma once

#include "EngineParams.h"
#include "World.h"
#include "InputState.h"

#include "Exports.h"

class InputLayer
{
public:
	REDFORGE_API virtual ~InputLayer();

	REDFORGE_API virtual void Startup(const EngineStartupParams& params, InputState& inputState) = 0;
	REDFORGE_API virtual void Shutdown(const EngineShutdownParams& params, InputState& inputState) = 0;

	REDFORGE_API virtual void PreUpdate(InputState& inputState);
	REDFORGE_API virtual void PostUpdate(InputState& inputState);
};