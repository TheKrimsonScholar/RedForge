#include "Texture.h"

#include "World.h"

#include "GraphicsState.h"

Texture::Texture()
{

}

void Texture::Startup(const EngineStartupParams& params, World& world)
{

}
void Texture::Shutdown(const EngineShutdownParams& params, World& world)
{
    const GraphicsState& graphicsState = world.GetResource<GraphicsState>();

    vkDestroyImageView(graphicsState.device, textureImageView, nullptr);

    vkDestroyImage(graphicsState.device, textureImage, nullptr);
    vkFreeMemory(graphicsState.device, textureImageMemory, nullptr);

    vkDestroySampler(graphicsState.device, textureSampler, nullptr);
}