#include "Mesh.h"

#include "World.h"

#include "GraphicsState.h"

Mesh::Mesh()
{

}

void Mesh::Startup(const EngineStartupParams& params, World& world)
{

}
void Mesh::Shutdown(const EngineShutdownParams& params, World& world)
{
    const GraphicsState& graphicsState = world.GetResource<GraphicsState>();

    vkDestroyBuffer(graphicsState.device, indexBuffer, nullptr);
    vkFreeMemory(graphicsState.device, indexBufferMemory, nullptr);

    vkDestroyBuffer(graphicsState.device, vertexBuffer, nullptr);
    vkFreeMemory(graphicsState.device, vertexBufferMemory, nullptr);
}