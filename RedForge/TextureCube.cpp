#include "TextureCube.h"

#include "GraphicsSystem.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

TextureCube::TextureCube()
{

}

void TextureCube::Startup(const EngineStartupParams& params, World& world)
{

}
void TextureCube::Shutdown(const EngineShutdownParams& params, World& world)
{
    const GraphicsState& graphicsState = world.GetResource<GraphicsState>();

    vkDestroyImageView(graphicsState.device, textureImageView, nullptr);

    vkDestroyImage(graphicsState.device, textureImage, nullptr);
    vkFreeMemory(graphicsState.device, textureImageMemory, nullptr);

    vkDestroySampler(graphicsState.device, sampler, nullptr);
}