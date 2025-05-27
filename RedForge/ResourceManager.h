#pragma once

#include <string>
#include <unordered_map>

#include "Texture.h"
#include "TextureCube.h"
#include "Material.h"
#include "Mesh.h"

#define TEXTURES_PATH (std::wstring) L"textures/"
#define TEXTURE_CUBES_PATH (std::wstring) L"textures/textureCubes/"
#define MATERIALS_PATH (std::wstring) L"materials/"
#define MESHES_PATH (std::wstring) L"meshes/"

class ResourceManager
{
private:
	static inline ResourceManager* Instance;

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

	static std::string GetExePath();
	static std::string FixPath(const std::string& relativeFilePath);
	static std::wstring FixPath(const std::wstring& relativeFilePath);
	static std::string WideToNarrow(const std::wstring& str);
	static std::wstring NarrowToWide(const std::string& str);

public:
	static Texture* GetTexture(const std::wstring& identifier) { return GetTexture(Instance->textureMap[identifier]->index); };
	static Texture* GetTexture(uint32_t index) { return Instance->textures[index]; };
	static TextureCube* GetTextureCube(const std::wstring& identifier) { return GetTextureCube(Instance->textureCubeMap[identifier]->index); };
	static TextureCube* GetTextureCube(uint32_t index) { return Instance->textureCubes[index]; };
	static Material* GetMaterial(const std::wstring& identifier) { return GetMaterial(Instance->materialMap[identifier]->index); };
	static Material* GetMaterial(uint32_t index) { return Instance->materials[index]; };
	static Mesh* GetMesh(const std::wstring& identifier) { return GetMesh(Instance->meshMap[identifier]->index); };
	static Mesh* GetMesh(uint32_t index) { return Instance->meshes[index]; };

	static std::vector<Texture*> GetTextures() { return Instance->textures; };
	static std::vector<Material*> GetMaterials() { return Instance->materials; };
	static std::vector<Mesh*> GetMeshes() { return Instance->meshes; };
};