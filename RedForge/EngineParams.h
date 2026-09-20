#pragma once

#include <filesystem>

#include <vulkan/vulkan.h>

struct EngineStartupParams
{
	VkSurfaceKHR surfaceOverride = VK_NULL_HANDLE;

	// The path to the level to load on engine startup. If this is an IP address, the engine will attempt to connect to a server at that address instead of loading a level.
	std::filesystem::path startupLevelPath = {};

	// Returns true if startupLevelPath is an IP address, indicating that the engine should attempt to connect to a server instead of loading a level.
	bool ShouldConnectToServer() const
	{
		std::string pathStr = startupLevelPath.string();
		bool isIPv4 = std::count(pathStr.begin(), pathStr.end(), '.') == 3; // If the string has 3 dots, consider it an IPv4 address
		bool isIPv6 = std::count(pathStr.begin(), pathStr.end(), ':') >= 2; // If the string has at least 2 colons, consider it an IPv6 address

		return isIPv4 || isIPv6;
	}

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