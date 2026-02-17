#pragma once

#include "System.h"

#include <string>
#include <unordered_map>

#include "GraphicsSystem.h"

#include "Assets.h"
#include "GraphicsState.h"

#include "FileManager.h"

#include "Texture.h"
#include "TextureCube.h"
#include "Material.h"
#include "Mesh.h"

#include "Exports.h"

REDFORGE_API std::filesystem::path GetEngineTexturesPath();
REDFORGE_API std::filesystem::path GetEngineTextureCubesPath();
REDFORGE_API std::filesystem::path GetEngineMaterialsPath();
REDFORGE_API std::filesystem::path GetEngineMeshesPath();

class ResourceManager : public System<>
{
private:
	REDFORGE_API static inline ResourceManager* Instance;

public:
	ResourceManager() {};
	~ResourceManager() {};

	void Startup(const EngineStartupParams& params, World& world) override;
	void PostStartup(const EngineStartupParams& params, World& world) override;
	void Shutdown(const EngineShutdownParams& params, World& world) override;

	void Update(SystemContext<>& ctx, float deltaTime) override;

	static void LoadAllTextures(Assets& assets, const GraphicsState& graphicsState);
	static void LoadAllMaterials(Assets& assets, const GraphicsState& graphicsState);
	static void LoadAllMeshes(Assets& assets, const GraphicsState& graphicsState);

	static const Texture& LoadTexture(Assets& assets, const GraphicsState& graphicsState, const std::filesystem::path& filePath);
	static const TextureCube& LoadTextureCube(Assets& assets, const GraphicsState& graphicsState, const std::filesystem::path& filePath);
	static const Material& LoadMaterial(Assets& assets, const std::filesystem::path& filePath);
	static const Mesh& LoadModel(Assets& assets, const GraphicsState& graphicsState, const std::filesystem::path& filePath);

private:
	static void CreateTextureImage(const GraphicsState& graphicsState, Texture& texture, const std::filesystem::path& filePath);
	static void CreateTextureCubeImage(const GraphicsState& graphicsState, TextureCube& textureCube, const std::array<std::filesystem::path, 6>& filePaths);
	static void GenerateMipmaps(const GraphicsState& graphicsState, VkImage image, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight, uint32_t mipLevels);
	static void CreateTextureSampler(const GraphicsState& graphicsState, Texture& texture);
	static void CreateTextureCubeSampler(const GraphicsState& graphicsState, TextureCube& textureCube);
	static VkImageView CreateImageView(const GraphicsState& graphicsState, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t layerCount = 1);
};
REGISTER_SYSTEM_BEGIN(ResourceManager)
SYSTEM_REQUIRES(GraphicsSystem)
REGISTER_SYSTEM_END(ResourceManager)