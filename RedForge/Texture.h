#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Texture
{
public:
    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VkImageView textureImageView;

    VkSampler textureSampler;
    
    // Index of the texture in the global textures array, assigned by the graphics system.
    uint32_t index = -1;

public:
    Texture();
    ~Texture();
};