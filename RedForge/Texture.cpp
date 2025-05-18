#include "Texture.h"

#include "GraphicsSystem.h"

Texture::Texture()
{

}
Texture::~Texture()
{
    vkDestroyImageView(GraphicsSystem::GetDevice(), textureImageView, nullptr);

    vkDestroyImage(GraphicsSystem::GetDevice(), textureImage, nullptr);
    vkFreeMemory(GraphicsSystem::GetDevice(), textureImageMemory, nullptr);

    vkDestroySampler(GraphicsSystem::GetDevice(), textureSampler, nullptr);
}