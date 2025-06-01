#pragma once

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

#include "Exports.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const uint32_t MAX_INSTANCES = 256;
const uint32_t MAX_LIGHTS = 256;

const uint32_t MAX_TEXTURES = 256;
const uint32_t MAX_MATERIALS = 256;

const std::vector<const char*> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    "VK_EXT_descriptor_indexing"
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

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
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

REDFORGE_API class GraphicsSystem
{
private:
    static inline REDFORGE_API GraphicsSystem* Instance;

    GLFWwindow* window;

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

    void Startup();
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
    void RecreateSwapChain();

    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void UpdateUniformBuffer(uint32_t currentImage);

public:
    static REDFORGE_API GLFWwindow* GetWindow() { return Instance->window; };
    static uint32_t GetWindowWidth() { return Instance->swapChainExtent.width; };
    static uint32_t GetWindowHeight() { return Instance->swapChainExtent.height; };
    static REDFORGE_API float GetAspectRatio() { return (float) Instance->swapChainExtent.width / (float) Instance->swapChainExtent.height; };
    static VkPhysicalDevice GetPhysicalDevice() { return Instance->physicalDevice; };
    static VkDevice GetDevice() { return Instance->device; };

	static void SetSkyboxTextureCube(TextureCube* textureCube) { Instance->skyboxTextureCube = textureCube; };
};