#pragma once

#include "ResourceRegistrationMacros.h"

#include "Texture.h"
#include "TextureCube.h"
#include "Material.h"
#include "Mesh.h"

struct Assets : public IResource
{
private:
	std::vector<Texture*> textures;
	std::vector<TextureCube*> textureCubes;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;

	std::unordered_map<std::wstring, Texture*> textureMap;
	std::unordered_map<std::wstring, TextureCube*> textureCubeMap;
	std::unordered_map<std::wstring, Material*> materialMap;
	std::unordered_map<std::wstring, Mesh*> meshMap;

private:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;
	
public:
	Texture& CreateTexture(const std::wstring& identifier);
	TextureCube& CreateTextureCube(const std::wstring& identifier);
	Material& CreateMaterial(const std::wstring& identifier);
	Mesh& CreateMesh(const std::wstring& identifier);

	void ShutdownAllAssets(const EngineShutdownParams& params, World& world);

	Texture& GetTexture(const std::wstring& identifier) { return *textureMap.at(identifier); };
	const Texture& GetTexture(const std::wstring& identifier) const { return *textureMap.at(identifier); };
	
	Texture& GetTexture(uint32_t index) { return *textures[index]; };
	const Texture& GetTexture(uint32_t index) const { return *textures[index]; };
	
	TextureCube& GetTextureCube(const std::wstring& identifier) { return *textureCubeMap.at(identifier); };
	const TextureCube& GetTextureCube(const std::wstring& identifier) const { return *textureCubeMap.at(identifier); };

	TextureCube& GetTextureCube(uint32_t index) { return *textureCubes[index]; };
	const TextureCube& GetTextureCube(uint32_t index) const { return *textureCubes[index]; };
	
	Material& GetMaterial(const std::wstring& identifier) { return *materialMap.at(identifier); };
	const Material& GetMaterial(const std::wstring& identifier) const { return *materialMap.at(identifier); };
	
	Material& GetMaterial(uint32_t index) { return *materials[index]; };
	const Material& GetMaterial(uint32_t index) const { return *materials[index]; };

	Mesh& GetMesh(const std::wstring& identifier) { return *meshMap.at(identifier); };
	const Mesh& GetMesh(const std::wstring& identifier) const { return *meshMap.at(identifier); };
	
	Mesh& GetMesh(uint32_t index) { return *meshes[index]; };
	const Mesh& GetMesh(uint32_t index) const { return *meshes[index]; };

	REDFORGE_API std::vector<Texture*> GetTextures() const;
	REDFORGE_API std::vector<TextureCube*> GetTextureCubes() const;
	REDFORGE_API std::vector<Material*> GetMaterials() const;
	REDFORGE_API std::vector<Mesh*> GetMeshes() const;

	bool HasTexture(const std::wstring& identifier) const { return textureMap.find(identifier) != textureMap.end(); }
	bool HasTextureCube(const std::wstring& identifier) const { return textureCubeMap.find(identifier) != textureCubeMap.end(); }
	bool HasMaterial(const std::wstring& identifier) const { return materialMap.find(identifier) != materialMap.end(); }
	bool HasMesh(const std::wstring& identifier) const { return meshMap.find(identifier) != meshMap.end(); }

	friend struct RegisterResourceBase;
};
REGISTER_RESOURCE(Assets)