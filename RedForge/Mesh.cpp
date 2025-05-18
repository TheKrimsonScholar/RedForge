#include "Mesh.h"

#include "GraphicsSystem.h"

Mesh::Mesh()
{

}
Mesh::~Mesh()
{
    vkDestroyBuffer(GraphicsSystem::GetDevice(), indexBuffer, nullptr);
    vkFreeMemory(GraphicsSystem::GetDevice(), indexBufferMemory, nullptr);

    vkDestroyBuffer(GraphicsSystem::GetDevice(), vertexBuffer, nullptr);
    vkFreeMemory(GraphicsSystem::GetDevice(), vertexBufferMemory, nullptr);
}