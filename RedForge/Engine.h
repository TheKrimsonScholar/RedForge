#pragma once

#include "EngineParams.h"

#include "Scheduler.h"

#include "DebugMacros.h"

#include "Exports.h"

#include "Archetype.h"

REDFORGE_API class Engine
{
private:
	World world;
	Scheduler scheduler;

	std::vector<SystemBase*> archetypes;

	bool isRunning = false;

public:
	REDFORGE_API Engine() {};
	REDFORGE_API ~Engine() {};

	REDFORGE_API EngineStartupParams CreateStartupParams() { return EngineStartupParams(); }
	REDFORGE_API EngineShutdownParams CreateShutdownParams() { return EngineShutdownParams(); }

	REDFORGE_API void Run();

	// Create the Vulkan instance to be used by the graphics system. Should only be called if the instance is needed before engine initialization.
	REDFORGE_API void CreateVulkanInstance();
	REDFORGE_API void DestroyVulkanInstance();

	REDFORGE_API void Startup(const EngineStartupParams& params = {});
	REDFORGE_API void Shutdown(const EngineShutdownParams& params = {});

	REDFORGE_API void Update(float deltaTime);

	REDFORGE_API bool IsRunning() { return isRunning; };

	/*template<typename... Args, typename Callback>
	REDFORGE_API void QueueExternalTask(Callback&& callback);*/

	REDFORGE_API void QueueExternalTask(std::function<void(World&)> callback);

	REDFORGE_API World& GetWorld() { return world; }
};

//#ifdef REDFORGE_EXPORTS
//	template<typename... Args, typename Callback>
//	void Engine::QueueExternalTask(Callback&& callback)
//	{
//		return scheduler.QueueExternalTask(callback);
//	}
//#endif