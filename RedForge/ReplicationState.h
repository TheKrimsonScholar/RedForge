#pragma once

#include "ResourceRegistration.h"

struct ReplicationState : public IResource
{
	SERIALIZED_RESOURCE_VARS()
	REPLICATED_RESOURCE_VARS()

protected:
	void Startup(const EngineStartupParams& params, World& world) override;
	void Shutdown(const EngineShutdownParams& params, World& world) override;
};
REGISTER_RESOURCE_TYPE(ReplicationState, IResource)