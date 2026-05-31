#pragma once

#include "System.h"
#include "SystemRegistrationMacros.h"

#include "TimeResource.h"

#include "Exports.h"

class TimeManager : public System<Time>
{
public:
	TimeManager();
	~TimeManager();

	void Startup(const EngineStartupParams& params, World& world) override;
	void PostStartup(const EngineStartupParams& params, World& world) override;
	void Shutdown(const EngineShutdownParams& params, World& world) override;

	void Update(SystemContext<Time>& ctx, float deltaTime) override;
};