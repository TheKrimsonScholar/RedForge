#pragma once

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

const int MAX_FRAMES_IN_FLIGHT = 1;

const uint32_t MAX_INSTANCES = 256;
const uint32_t MAX_LIGHTS = 256;

const uint32_t MAX_TEXTURES = 256;
const uint32_t MAX_MATERIALS = 256;

const VkFormat EXTERNAL_RENDER_IMAGE_FORMAT = VK_FORMAT_R8G8B8A8_UNORM; // Match with OpenGL

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
    VK_KHR_SURFACE_EXTENSION_NAME
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

struct MaterialData
{
    uint32_t albedoIndex;
    uint32_t normalsIndex;
    uint32_t roughnessIndex;
    uint32_t metalnessIndex;
};

struct InstanceData
{
    // Stable index used for accessing all per-object data.
    uint32_t rendererIndex;

    //uint32_t transformIndex;
    uint32_t meshIndex;
    uint32_t materialIndex;
};

struct alignas(16) LightData
{
    glm::vec3 direction;
    ELightType lightType;
    glm::vec3 color;
    float intensity;
    glm::vec3 location;
    float range;
    float spotInnerAngle;
    float spotOuterAngle;

    float _PADDING[2];
};
struct LightBufferData
{
    uint32_t lightCount;
    uint32_t _padding[3];
    LightData lights[MAX_LIGHTS];
};

struct DrawBatch
{
    uint32_t meshIndex;
    uint32_t instanceOffset;
    uint32_t instanceCount;
};

class GraphicsSystem
{
private:
    static inline REDFORGE_API GraphicsSystem* Instance;

    GLFWwindow* window;
    GLFWInputLayer inputLayer;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    // Does the graphics system render to a custom render target instead of directly rendering to the swapchain?
    bool shouldRenderOffscreen = false;
    VkExtent2D externalRenderImageExtent;
    VkImage externalRenderImage;
    VkDeviceMemory externalRenderMemory;
    size_t externalRenderMemorySize = 0;
    VkImageView externalRenderImageView;
    VkFramebuffer externalRenderFramebuffer;
    VkSemaphore externalRenderCompleteSemaphore;
    VkSemaphore externalRenderReleaseSemaphore;
    #ifdef _WIN32
        HANDLE externalRenderMemoryHandle = INVALID_HANDLE_VALUE;
        HANDLE externalRenderCompleteSemaphoreHandle = INVALID_HANDLE_VALUE;
        HANDLE externalRenderReleaseSemaphoreHandle = INVALID_HANDLE_VALUE;

        // Function pointers for extension functions
        PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR = nullptr;
        PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR = nullptr;
    #else
	    int externalRenderMemoryFd = -1;
	    int externalRenderCompleteSemaphoreFd = -1;
	    int externalRenderReleaseSemaphoreFd = -1;

	    // Function pointers for extension functions
	    PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR = nullptr;
	    PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR = nullptr;
    #endif

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipelineLayout skyboxPipelineLayout;
    VkPipelineLayout debugPipelineLayout;

    VkPipeline graphicsPipeline;
    VkPipeline skyboxGraphicsPipeline;
    VkPipeline debugGraphicsPipeline;

    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    bool framebufferResized = false;

    uint32_t currentFrame = 0;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    VkBuffer materialsBuffer[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory materialsBufferMemory[MAX_FRAMES_IN_FLIGHT];
    void* materialsBufferMapped[MAX_FRAMES_IN_FLIGHT];

    VkBuffer transformBuffer[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory transformBufferMemory[MAX_FRAMES_IN_FLIGHT];
    void* transformBufferMapped[MAX_FRAMES_IN_FLIGHT];

    VkBuffer instancesBuffer[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory instancesBufferMemory[MAX_FRAMES_IN_FLIGHT];
    void* instancesBufferMapped[MAX_FRAMES_IN_FLIGHT];

    VkBuffer cameraUBOs[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory cameraUBOsMemory[MAX_FRAMES_IN_FLIGHT];
    void* cameraUBOsMapped[MAX_FRAMES_IN_FLIGHT];

    VkBuffer lightsBuffer[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory lightsBufferMemory[MAX_FRAMES_IN_FLIGHT];
    void* lightsBufferMapped[MAX_FRAMES_IN_FLIGHT];

    VkDescriptorPool descriptorPool;
    VkDescriptorPool imguiDescriptorPool;
    //std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];

    std::vector<VkDescriptorImageInfo> texturesBufferData;
    std::vector<MaterialData> materialsBufferData;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    glm::mat4 modelMatrices[MAX_INSTANCES];

    std::vector<InstanceData> instanceData;
    std::vector<MeshRendererComponent*> renderers;
    uint32_t rendererInstances[MAX_INSTANCES];
    std::queue<uint32_t> renderersFreeList;
    uint32_t renderersNextIndex = 0;

    TextureCube* skyboxTextureCube;

    std::vector<DrawBatch> drawBatches;

public:
    GraphicsSystem() {};
    ~GraphicsSystem() {};

    void Startup(bool shouldOverrideFramebuffer = false, unsigned int overrideExtentWidth = 0, unsigned int overrideExtentHeight = 0);
    void Shutdown();

    REDFORGE_API void Update();
    
    static void CreateVertexBuffer(std::vector<Vertex> vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);
    static void CreateIndexBuffer(std::vector<uint32_t> indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);
    
    static void CreateBuffer(
        VkDeviceSize size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties, 
        VkBuffer& buffer, 
        VkDeviceMemory& bufferMemory);
    static void CreateImage(
        uint32_t width, uint32_t height, uint32_t mipLevels, 
        VkSampleCountFlagBits numSamples, 
        VkFormat format, 
        VkImageTiling tiling, 
        VkImageUsageFlags usage, 
        VkMemoryPropertyFlags properties, 
        VkImage& image, 
        VkDeviceMemory& imageMemory,
        VkImageCreateFlags flags = 0, uint32_t arrayLayers = 1);
    static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, 
        uint32_t layerCount = 1);
    static void CopyImageToBuffer(VkImage image, VkFormat format, VkExtent2D extent, VkBuffer buffer, VkCommandPool commandPool, VkQueue queue);
    
    static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, 
        uint32_t layerCount = 1);
    
    static VkCommandBuffer BeginSingleTimeCommands();
    static void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

private:
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    void InitWindow();
    void InitVulkan();
    void InitImGui();

    void CreateInstance();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    void LoadExtensionFunctions();

    void SetupDebugMessenger();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void CreateSurface();

    void SelectPhysicalDevice();
    int RateDeviceSuitability(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSampleCountFlagBits GetMaxUsableSampleCount();

    void CreateLogicalDevice();

    void CreateSwapChain();
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void CreateImageViews();

    void CreateRenderPass();

    void CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();
    void CreateSkyboxGraphicsPipeline();
    void CreateDebugGraphicsPipeline();
    VkShaderModule CreateShaderModule(const std::vector<char>& code);

    void CreateFramebuffers();

    void CreateCommandPool();

    void CreateColorResources();
    void CreateDepthResources();
    VkFormat FindDepthFormat();
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    bool HasStencilComponent(VkFormat format);

    void CreateGlobalBuffers();
    void CreateGlobalArrayDescriptorSets();
    void UpdateMaterialsBuffer();
    void UpdateGlobalArrays(uint32_t currentImage);

    void CreateTextureImageView(Texture* texture);

    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, 
        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t layerCount = 1);

    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateImGuiDescriptorPool();
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateCommandBuffers();

    void CreateSyncObjects();

    void CleanupSwapChain();

public:
    REDFORGE_API static void RecreateSwapChain();
    
private:
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void UpdateUniformBuffer(uint32_t currentImage);

public:
    REDFORGE_API static VkInstance GetVulkanInstance() { return Instance->instance; };
    REDFORGE_API static GLFWwindow* GetWindow() { return Instance->window; };
    static VkExtent2D GetRenderExtent() { return Instance->shouldRenderOffscreen ? Instance->externalRenderImageExtent : Instance->swapChainExtent; };
    static uint32_t GetWindowWidth() { return GetRenderExtent().width; };
    static uint32_t GetWindowHeight() { return GetRenderExtent().height; };
    static VkFormat GetRenderFormat() { return Instance->shouldRenderOffscreen ? EXTERNAL_RENDER_IMAGE_FORMAT : Instance->swapChainImageFormat; };
    REDFORGE_API static float GetAspectRatio() { return (float) GetWindowWidth() / (float) GetWindowHeight(); };
    static VkPhysicalDevice GetPhysicalDevice() { return Instance->physicalDevice; };
    static VkDevice GetDevice() { return Instance->device; };

    static void SetSurface(VkSurfaceKHR surface) { Instance->surface = surface; };

	static void SetSkyboxTextureCube(TextureCube* textureCube) { Instance->skyboxTextureCube = textureCube; };

    void CreateExternalRenderSyncObjects();
    void CreateExternalRenderResources();
    void CreateExternalRenderImage();
    void CreateExternalRenderImageView();
    void CreateExternalRenderFramebuffer();

    static size_t GetExternalRenderMemorySize() { return Instance->externalRenderMemorySize; };
    #ifdef _WIN32
        static void* GetExternalRenderMemoryHandle() { return Instance->externalRenderMemoryHandle; };
        static void* GetExternalRenderCompleteSemaphoreHandle() { return Instance->externalRenderCompleteSemaphoreHandle; };
        static void* GetExternalRenderReleaseSemaphoreHandle() { return Instance->externalRenderReleaseSemaphoreHandle; };
    #else
	    static int GetExternalRenderMemoryHandle() { return Instance->externalRenderMemoryFd; };
	    static int GetExternalRenderCompleteSemaphoreHandle() { return Instance->externalRenderCompleteSemaphoreFd; };
	    static int GetExternalRenderReleaseSemaphoreHandle() { return Instance->externalRenderReleaseSemaphoreFd; };
    #endif

    void CleanupExternalRenderResources();

    REDFORGE_API static void SetExternalRenderImageExtent(uint32_t width, uint32_t height) { if(Instance) Instance->externalRenderImageExtent = { width, height }; };
};