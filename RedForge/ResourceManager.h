#pragma once

#include <string>

#include "Texture.h"
#include "Material.h"
#include "Mesh.h"

class ResourceManager
{
private:
	static inline ResourceManager* Instance;

	std::vector<Texture*> textures;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;

public:
	ResourceManager() {};
	~ResourceManager() {};

	void Startup();
	void Shutdown();

	static void AddMaterial(Texture* texture);

	static Texture* LoadTexture(const std::wstring& filePath);
	static Mesh* LoadModel(const std::wstring& filePath);

	static std::vector<char> ReadFile(const std::string& filename);

private:
	static void CreateTextureImage(Texture* texture, const std::wstring& filePath);
	static void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight, uint32_t mipLevels);
	static void CreateTextureSampler(Texture* texture);
	static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

public:
	static Texture* GetTexture(uint32_t index) { return Instance->textures[index]; };
	static Material* GetMaterial(uint32_t index) { return Instance->materials[index]; };
	static Mesh* GetMesh(uint32_t index) { return Instance->meshes[index]; };

	static std::vector<Texture*> GetTextures() { return Instance->textures; };
	static std::vector<Material*> GetMaterials() { return Instance->materials; };
	static std::vector<Mesh*> GetMeshes() { return Instance->meshes; };
};