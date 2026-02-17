#pragma once

#include "Asset.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct TextureCube : public IAsset
{
public:
    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VkImageView textureImageView;

    VkSampler sampler;

    // Index of the texture cube in the global texture cubes array, assigned by the graphics system.
    uint32_t index = -1;

private:
    TextureCube();

    virtual void Startup(const EngineStartupParams& params, World& world) override;
    virtual void Shutdown(const EngineShutdownParams& params, World& world) override;

    friend struct Assets;
};