#pragma once

#include "System.h"

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
#else
    //#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vector>
#include <cstring>
#include <optional>
#include <fstream>
#include <queue>

#include "WindowSystem.h"

#include "Window.h"
#include "GraphicsState.h"
#include "Cameras.h"
#include "Assets.h"
#include "TimeResource.h"

#include "Vertex.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "TextureCube.h"
#include "MeshRendererComponent.h"
#include "LightComponent.h"
#include "GLFWInputLayer.h"

#include "Exports.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #include <vulkan/vulkan_win32.h>
#else
    //#include <vulkan/vulkan_xlib.h>
#endif

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const uint32_t MAX_TEXTURES = 256;
const uint32_t MAX_MATERIALS = 256;

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
    #ifdef _WIN32
        VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME,
    #else
        VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME,
    #endif
    "VK_EXT_descriptor_indexing"
};
const std::vector<const char*> instanceExtensions =
{
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    //"VK_KHR_external_memory_win32",
    //"GL_EXT_memory_object_win32"
};
const std::vector<const char*> validationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class GraphicsSystem : public System<Window, GraphicsState, const Cameras, const Assets, const Time, TransformComponent, MeshRendererComponent, const LightComponent, const CameraComponent>
{
private:
    static inline REDFORGE_API GraphicsSystem* Instance;

public:
    GraphicsSystem() {};
    ~GraphicsSystem() {};

    void CreateVulkanInstance_PreStartup(GraphicsState& graphicsState);
    void DestroyVulkanInstance_PostShutdown(const GraphicsState& graphicsState);

    REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
    REDFORGE_API void PostStartup(const EngineStartupParams& params, World& world) override;
    REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

    REDFORGE_API void Update(LocalSystemContext& ctx, float deltaTime) override;

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    void InitVulkan(Window& window, GraphicsState& graphicsState);
    void InitImGui(const Window& window, GraphicsState& graphicsState);

    void CreateVulkanInstance(GraphicsState& graphicsState);
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();

    void SetupDebugMessenger(GraphicsState& graphicsState);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void CreateSurface(const Window& window, GraphicsState& graphicsState);

    void SelectPhysicalDevice(const Window& window, GraphicsState& graphicsState);
    int RateDeviceSuitability(const Window& window, const GraphicsState& graphicsState, VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(const Window& window, const GraphicsState& graphicsState, VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(const GraphicsState& graphicsState, VkPhysicalDevice device);
    VkSampleCountFlagBits GetMaxUsableSampleCount(const GraphicsState& graphicsState);

    void CreateLogicalDevice(const Window& window, GraphicsState& graphicsState);

    void CreateSwapChain(Window& window, const GraphicsState& graphicsState);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities);

    void CreateImageViews(Window& window, const GraphicsState& graphicsState);

    void CreateRenderPass(const Window& window, GraphicsState& graphicsState);

    void CreateDescriptorSetLayout(GraphicsState& graphicsState);
    void CreateGraphicsPipeline(const Window& window, GraphicsState& graphicsState);
    void CreateSkyboxGraphicsPipeline(const Window& window, GraphicsState& graphicsState);
    void CreateDebugGraphicsPipeline(const Window& window, GraphicsState& graphicsState);
    VkShaderModule CreateShaderModule(const GraphicsState& graphicsState, const std::vector<char>& code);

    void CreateFramebuffers(Window& window, const GraphicsState& graphicsState);

    void CreateCommandPool(const Window& window, GraphicsState& graphicsState);

    void CreateColorResources(const Window& window, GraphicsState& graphicsState);
    void CreateDepthResources(const Window& window, GraphicsState& graphicsState);
    VkFormat FindDepthFormat(const GraphicsState& graphicsState);
    VkFormat FindSupportedFormat(const GraphicsState& graphicsState, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    bool HasStencilComponent(VkFormat format);

    void CreateGlobalBuffers(GraphicsState& graphicsState);
    void CreateGlobalArrayDescriptorSets(const Assets& assets, GraphicsState& graphicsState);
    void UpdateMaterialsBuffer(const Assets& assets, GraphicsState& graphicsState);
    void UpdateGlobalArrays(uint32_t currentImage);

    void CreateTextureImageView(const GraphicsState& graphicsState, Texture* texture);

    VkImageView CreateImageView(const GraphicsState& graphicsState, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t layerCount = 1);

    void CreateUniformBuffers(GraphicsState& graphicsState);
    void CreateDescriptorPool(GraphicsState& graphicsState);
    void CreateImGuiDescriptorPool(GraphicsState& graphicsState);
    uint32_t FindMemoryType(const GraphicsState& graphicsState, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateCommandBuffers(GraphicsState& graphicsState);

    void CreateSyncObjects(GraphicsState& graphicsState);

    void CleanupSwapChain(const Window& window, const GraphicsState& graphicsState);

    void DestroyVulkanInstance(const GraphicsState& graphicsState);

public:
    REDFORGE_API void RecreateSwapChain(Window& window, GraphicsState& graphicsState);
    
private:
    void RecordCommandBuffer(LocalSystemContext& ctx, VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void UpdateUniformBuffer(const GraphicsState& graphicsState, const Cameras& cameras, SystemContext<const TransformComponent, const CameraComponent> ctx, uint32_t currentImage);
};
REGISTER_SYSTEM_BEGIN(GraphicsSystem)
SYSTEM_REQUIRES(WindowSystem)
REGISTER_SYSTEM_END(GraphicsSystem)