#include "Texture.h"

#include "World.h"

#include "GraphicsState.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

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