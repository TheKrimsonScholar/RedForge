#pragma once

#include "ResourceRegistration.h"

#include <queue>

struct DebugState : public IResource
{
	SERIALIZED_RESOURCE_VARS()
	REPLICATED_RESOURCE_VARS()

//private:
	//static inline std::queue<LogMessage> debugLogQueue;

protected:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;
};
REGISTER_RESOURCE_TYPE(DebugState, IResource)