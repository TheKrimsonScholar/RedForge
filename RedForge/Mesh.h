#pragma once

#include <vector>

#include "Vertex.h"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    // Index of the mesh in the global meshes array, assigned by the graphics system.
    uint32_t index = -1;

public:
    Mesh();
    ~Mesh();
};