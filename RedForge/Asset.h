#pragma once

#include "EngineParams.h"

class World;

class IAsset
{
	virtual void Startup(const EngineStartupParams& params, World& world) = 0;
	virtual void Shutdown(const EngineShutdownParams& params, World& world) = 0;

	friend class Assets;
};