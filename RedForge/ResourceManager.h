#pragma once

#include <string>
#include <unordered_map>

#include "Texture.h"
#include "TextureCube.h"
#include "Material.h"
#include "Mesh.h"

#include "Exports.h"

#define TEXTURES_PATH (std::wstring) L"textures/"
#define TEXTURE_CUBES_PATH (std::wstring) L"textures/textureCubes/"
#define MATERIALS_PATH (std::wstring) L"materials/"
#define MESHES_PATH (std::wstring) L"meshes/"

REDFORGE_API class ResourceManager
{
private:
	REDFORGE_API static inline ResourceManager* Instance;

	std::vector<Texture*> textures;
	std::vector<TextureCube*> textureCubes;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;

	std::unordered_map<std::wstring, Texture*> textureMap;
	std::unordered_map<std::wstring, TextureCube*> textureCubeMap;
	std::unordered_map<std::wstring, Material*> materialMap;
	std::unordered_map<std::wstring, Mesh*> meshMap;

public:
	ResourceManager() {};
	~ResourceManager() {};

	void Startup();
	void Shutdown();

	static void LoadAllTextures();
	static void LoadAllMaterials();
	static void LoadAllMeshes();

	static Texture* LoadTexture(const std::wstring& filePath);
	static TextureCube* LoadTextureCube(const std::wstring& filePath);
	static Material* LoadMaterial(const std::wstring& filePath);
	static Mesh* LoadModel(const std::wstring& filePath);

	static std::vector<char> ReadFile(const std::string& filename);

private:
	static void CreateTextureImage(Texture* texture, const std::wstring& filePath);
	static void CreateTextureCubeImage(TextureCube* textureCube, const std::array<std::wstring, 6>& filePaths);
	static void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight, uint32_t mipLevels);
	static void CreateTextureSampler(Texture* texture);
	static void CreateTextureCubeSampler(TextureCube* textureCube);
	static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, 
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, uint32_t layerCount = 1);

	static std::unordered_map<std::wstring, std::wstring> GetAllFilesInDirectory(std::wstring directory, std::vector<std::wstring> extensions);

public:
	static Texture* GetTexture(const std::wstring& identifier) { return Instance->textureMap[identifier]; };
	static Texture* GetTexture(uint32_t index) { return Instance->textures[index]; };
	static TextureCube* GetTextureCube(const std::wstring& identifier) { return Instance->textureCubeMap[identifier]; };
	static TextureCube* GetTextureCube(uint32_t index) { return Instance->textureCubes[index]; };
	static Material* GetMaterial(const std::wstring& identifier) { return Instance->materialMap[identifier]; };
	static Material* GetMaterial(uint32_t index) { return Instance->materials[index]; };
	static Mesh* GetMesh(const std::wstring& identifier) { return Instance->meshMap[identifier]; };
	static Mesh* GetMesh(uint32_t index) { return Instance->meshes[index]; };

	static std::vector<Texture*> GetTextures() { return Instance->textures; };
	static std::vector<TextureCube*> GetTextureCubes() { return Instance->textureCubes; };
	static std::vector<Material*> GetMaterials() { return Instance->materials; };
	static std::vector<Mesh*> GetMeshes() { return Instance->meshes; };

	static std::unordered_map<std::wstring, Texture*> GetTextureMap() { return Instance->textureMap; };
	static std::unordered_map<std::wstring, TextureCube*> GetTextureCubeMap() { return Instance->textureCubeMap; };
	static std::unordered_map<std::wstring, Material*> GetMaterialMap() { return Instance->materialMap; };
	static std::unordered_map<std::wstring, Mesh*> GetMeshMap() { return Instance->meshMap; };
};