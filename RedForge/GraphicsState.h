#pragma once

#include "ResourceRegistrationMacros.h"

#include "Vertex.h"
#include "TextureCube.h"
#include "MeshRendererComponent.h"
#include "LightComponent.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const int MAX_FRAMES_IN_FLIGHT = 1;

const uint32_t MAX_INSTANCES = 256;
const uint32_t MAX_LIGHTS = 256;

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

struct GraphicsState : public IResource
{
    VkInstance vulkanInstance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipelineLayout skyboxPipelineLayout;
    VkPipelineLayout debugPipelineLayout;

    VkPipeline graphicsPipeline;
    VkPipeline skyboxGraphicsPipeline;
    VkPipeline debugGraphicsPipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

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

private:
    REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
    REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

public:
    void CreateVertexBuffer(std::vector<Vertex> vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory) const;
    void CreateIndexBuffer(std::vector<uint32_t> indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory) const;

    void CreateBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory) const;
    void CreateImage(
        uint32_t width, uint32_t height, uint32_t mipLevels,
        VkSampleCountFlagBits numSamples,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory,
        VkImageCreateFlags flags = 0, uint32_t arrayLayers = 1) const;
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount = 1) const;
    void CopyImageToBuffer(VkImage image, VkFormat format, VkExtent2D extent, VkBuffer buffer, VkCommandPool commandPool, VkQueue queue) const;

    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layerCount = 1) const;

    VkCommandBuffer BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

private:
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
};
REGISTER_RESOURCE(GraphicsState)