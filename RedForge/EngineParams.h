#pragma once

#include <vulkan/vulkan.h>

struct EngineStartupParams
{
	VkSurfaceKHR surfaceOverride = VK_NULL_HANDLE;

private:
	EngineStartupParams() {};

	friend class Engine;
};
struct EngineShutdownParams
{
	bool shouldDestroyVulkanInstance = true;

private:
	EngineShutdownParams() {};

	friend class Engine;
};