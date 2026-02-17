#pragma once

#include "Asset.h"

#include <vector>
#include <string>

#include "Vertex.h"
#include "Material.h"

class Mesh : public IAsset
{
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

	const Material* defaultMaterial;

    std::wstring identifier;
    // Index of the mesh in the global meshes array, assigned by the graphics system.
    uint32_t index = -1;

private:
    Mesh();

    REDFORGE_API virtual void Startup(const EngineStartupParams& params, World& world) override;
    REDFORGE_API virtual void Shutdown(const EngineShutdownParams& params, World& world) override;

    friend struct Assets;
};