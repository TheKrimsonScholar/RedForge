#include "TextureCube.h"

#include "GraphicsSystem.h"

TextureCube::TextureCube()
{

}
TextureCube::~TextureCube()
{
    vkDestroyImageView(GraphicsSystem::GetDevice(), textureImageView, nullptr);

    vkDestroyImage(GraphicsSystem::GetDevice(), textureImage, nullptr);
    vkFreeMemory(GraphicsSystem::GetDevice(), textureImageMemory, nullptr);

    vkDestroySampler(GraphicsSystem::GetDevice(), sampler, nullptr);
}