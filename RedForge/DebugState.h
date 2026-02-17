#pragma once

#include "ResourceRegistrationMacros.h"

#include <queue>

struct DebugState : public IResource
{
//private:
	//static inline std::queue<LogMessage> debugLogQueue;

private:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;
};
REGISTER_RESOURCE(DebugState)