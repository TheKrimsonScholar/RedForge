#include "Assets.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

void Assets::Startup(const EngineStartupParams& params, World& world)
{

}
void Assets::Shutdown(const EngineShutdownParams& params, World& world)
{
    
}

Texture& Assets::CreateTexture(const std::wstring& identifier)
{
    Texture* texture = new Texture();
    textures.push_back(texture);
	textureMap[identifier] = texture;

	texture->index = textures.size() - 1;

    return *texture;
}
TextureCube& Assets::CreateTextureCube(const std::wstring& identifier)
{
    TextureCube* textureCube = new TextureCube();
    textureCubes.push_back(textureCube);
    textureCubeMap[identifier] = textureCube;

	textureCube->index = textureCubes.size() - 1;

    return *textureCube;
}
Material& Assets::CreateMaterial(const std::wstring& identifier)
{
    Material* material = new Material();
    materials.push_back(material);
    materialMap[identifier] = material;

	material->index = materials.size() - 1;
	material->identifier = identifier;

    return *material;
}
Mesh& Assets::CreateMesh(const std::wstring& identifier)
{
    Mesh* mesh = new Mesh();
    meshes.push_back(mesh);
    meshMap[identifier] = mesh;

	mesh->index = meshes.size() - 1;
	mesh->identifier = identifier;

    return *mesh;
}

void Assets::ShutdownAllAssets(const EngineShutdownParams& params, World& world)
{
    for(Texture* texture : textures)
    {
		texture->Shutdown(params, world);
		delete texture;
    }
    textures.clear();
    textureMap.clear();

	for(TextureCube* textureCube : textureCubes)
    {
		textureCube->Shutdown(params, world);
		delete textureCube;
    }
	textureCubes.clear();
	textureCubeMap.clear();

    for(Material* material : materials)
    {
		material->Shutdown(params, world);
		delete material;
    }
    materials.clear();
    materialMap.clear();

	for(Mesh* mesh : meshes)
    {
		mesh->Shutdown(params, world);
		delete mesh;
    }
	meshes.clear();
	meshMap.clear();
}

std::vector<Texture*> Assets::GetTextures() const
{
    return textures;
}
std::vector<TextureCube*> Assets::GetTextureCubes() const
{
    return textureCubes;
}
std::vector<Material*> Assets::GetMaterials() const
{
    return materials;
}
std::vector<Mesh*> Assets::GetMeshes() const
{
    return meshes;
}