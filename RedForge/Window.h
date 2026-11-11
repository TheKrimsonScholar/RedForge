#pragma once

#include "ResourceRegistration.h"

#include "GLFWInputLayer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

struct Window : public IResource
{
	SERIALIZED_RESOURCE_VARS()
	REPLICATED_RESOURCE_VARS()

	GLFWwindow* window;
	GLFWInputLayer inputLayer;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

	// Does the graphics system render to a surface managed externally instead of using its own?
	bool isSurfaceOverridden = false;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	bool wasFramebufferResized = false;

protected:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

public:
    uint32_t GetWidth() const { return swapChainExtent.width; };
    uint32_t GetHeight() const { return swapChainExtent.height; };
    REDFORGE_API float GetAspectRatio() const { return (float) GetWidth() / (float) GetHeight(); };
};