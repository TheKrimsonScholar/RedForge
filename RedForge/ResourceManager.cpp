#include "ResourceManager.h"

#include <filesystem>
#include <unordered_map>
#include <iostream>

#include "GraphicsSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include "rapidobj/rapidobj.hpp"

void ResourceManager::Startup()
{
    Instance = this;
}
void ResourceManager::Shutdown()
{
    for(Mesh* mesh : meshes)
        delete mesh;
	meshes.clear();
    for(Material* material : materials)
        delete material;
	materials.clear();
	for(Texture* texture : textures)
		delete texture;
	textures.clear();
}

void ResourceManager::LoadAllTextures()
{
    const std::vector<std::wstring> VALID_TYPES = { L".png" };
    std::unordered_map<std::wstring, std::wstring> texturePaths = GetAllFilesInDirectory(TEXTURES_PATH, VALID_TYPES);
    
    // Load all textures from their paths and associate them with identifiers in an unordered map
	for(auto& texturePath : texturePaths)
        LoadTexture(TEXTURES_PATH + texturePath.first);
}
void ResourceManager::LoadAllMaterials()
{
	const std::vector<std::wstring> VALID_TYPES = { L".mtl" };
	std::unordered_map<std::wstring, std::wstring> materialPaths = GetAllFilesInDirectory(MATERIALS_PATH, VALID_TYPES);

	// Load all materials from their paths and associate them with identifiers in an unordered map
	for(auto& materialPath : materialPaths)
		LoadTexture(MATERIALS_PATH + materialPath.first);
}
void ResourceManager::LoadAllMeshes()
{
    const std::vector<std::wstring> VALID_TYPES = { L".obj" };
    std::unordered_map<std::wstring, std::wstring> meshPaths = GetAllFilesInDirectory(MESHES_PATH, VALID_TYPES);

    for(auto& meshPath : meshPaths)
        LoadModel(MESHES_PATH + meshPath.first);
}

Texture* ResourceManager::LoadTexture(const std::wstring& filePath)
{
	// Check if the texture is already loaded
	if(Instance->textureMap.find(filePath) != Instance->textureMap.end())
		return Instance->textureMap[filePath];

	Texture* texture = new Texture();

	CreateTextureImage(texture, filePath);

    texture->textureImageView = CreateImageView(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture->mipLevels);

	CreateTextureSampler(texture);

	texture->index = Instance->textures.size();
	Instance->textures.push_back(texture);
    Instance->textureMap.emplace(filePath.substr((TEXTURES_PATH).length()), texture);

	return texture;
}
Material* ResourceManager::LoadMaterial(const std::wstring& filePath)
{
    // Check if the material is already loaded
	if(Instance->materialMap.find(filePath) != Instance->materialMap.end())
		return Instance->materialMap[filePath];

    Material* material = new Material();

	/* --- Custom MAT parsing --- */

    material->index = Instance->materials.size();
    Instance->materials.push_back(material);
    Instance->materialMap.emplace(filePath.substr((MATERIALS_PATH).length()), material);

    return material;
}
Mesh* ResourceManager::LoadModel(const std::wstring& filePath)
{
    // Check if the mesh is already loaded
	if(Instance->meshMap.find(filePath) != Instance->meshMap.end())
		return Instance->meshMap[filePath];

    Mesh* mesh = new Mesh();

    rapidobj::Result obj = rapidobj::ParseFile(filePath.c_str());

    std::unordered_map<Vertex, uint32_t> vertexMap;

    // For each shape
    for(rapidobj::Shape& shape : obj.shapes)
    {
        uint32_t firstFaceIndex = 0;

        // For each face
        for(int f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            // For each vertex
            for(int j = 0; j < shape.mesh.num_face_vertices[f]; j++)
            {
                rapidobj::Index index = shape.mesh.indices[firstFaceIndex + j];

                Vertex vertex = {};
                vertex.pos = { obj.attributes.positions[index.position_index * 3], obj.attributes.positions[index.position_index * 3 + 1], obj.attributes.positions[index.position_index * 3 + 2] };
                //vertex.normal = { obj.attributes.normals[index.normal_index * 3], obj.attributes.normals[index.normal_index * 3 + 1], obj.attributes.normals[index.normal_index * 3 + 2] };
                vertex.uv = { obj.attributes.texcoords[index.texcoord_index * 2], 1.0f - obj.attributes.texcoords[index.texcoord_index * 2 + 1] };
                vertex.color = { 1.0f, 1.0f, 1.0f };

                if(vertexMap.count(vertex) == 0)
                {
                    vertexMap[vertex] = mesh->vertices.size();
                    mesh->vertices.push_back(vertex);
                }

                mesh->indices.push_back(vertexMap[vertex]);
            }

            firstFaceIndex += shape.mesh.num_face_vertices[f];
        }
    }
    // For each material associated with the .obj
    for(rapidobj::Material& mat : obj.materials)
    {
        if(Instance->materialMap.find(NarrowToWide(mat.name)) != Instance->materialMap.end())
        {
		    mesh->defaultMaterial = Instance->materialMap[NarrowToWide(mat.name)];
            continue;
        }

		Material* material = new Material();

		material->texture = LoadTexture(TEXTURES_PATH + NarrowToWide(mat.diffuse_texname));

        material->index = Instance->materials.size();
        Instance->materials.push_back(material);
        Instance->materialMap.emplace(NarrowToWide(mat.name), material);

		mesh->defaultMaterial = material;
    }

    GraphicsSystem::CreateVertexBuffer(mesh->vertices, mesh->vertexBuffer, mesh->vertexBufferMemory);
    GraphicsSystem::CreateIndexBuffer(mesh->indices, mesh->indexBuffer, mesh->indexBufferMemory);

	mesh->index = Instance->meshes.size();
    Instance->meshes.push_back(mesh);
    Instance->meshMap.emplace(filePath.substr((MESHES_PATH).length()), mesh);

    return mesh;
}

std::vector<char> ResourceManager::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open())
        throw std::runtime_error("Failed to open file! \"" + filename + "\"");

    // Determine size of the file to allocate a buffer
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // Seek back to the beginning and read all
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

void ResourceManager::CreateTextureImage(Texture* texture, const std::wstring& filePath)
{
	int textureWidth, textureHeight, textureChannels;
    stbi_uc* pixels = stbi_load(std::filesystem::path(filePath).string().c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = textureWidth * textureHeight * 4;
    texture->mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

    if(!pixels)
        throw std::runtime_error("Failed to load texture image!");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    GraphicsSystem::CreateBuffer(
        imageSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory);

    void* data;
    vkMapMemory(GraphicsSystem::GetDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(GraphicsSystem::GetDevice(), stagingBufferMemory);

    stbi_image_free(pixels);

    GraphicsSystem::CreateImage(
        textureWidth, textureHeight, texture->mipLevels, 
        VK_SAMPLE_COUNT_1_BIT, 
        VK_FORMAT_R8G8B8A8_SRGB, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        texture->textureImage, 
        texture->textureImageMemory);

    GraphicsSystem::TransitionImageLayout(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture->mipLevels);
    GraphicsSystem::CopyBufferToImage(stagingBuffer, texture->textureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
    //TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

    vkDestroyBuffer(GraphicsSystem::GetDevice(), stagingBuffer, nullptr);
    vkFreeMemory(GraphicsSystem::GetDevice(), stagingBufferMemory, nullptr);

    GenerateMipmaps(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, textureWidth, textureHeight, texture->mipLevels);
}
void ResourceManager::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(GraphicsSystem::GetPhysicalDevice(), imageFormat, &formatProperties);

    if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        throw std::runtime_error("Texture image format does not support linear blitting!");

    VkCommandBuffer commandBuffer = GraphicsSystem::BeginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = textureWidth;
    int32_t mipHeight = textureHeight;

    for(uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        if(mipWidth > 1) mipWidth /= 2;
        if(mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    
    GraphicsSystem::EndSingleTimeCommands(commandBuffer);
}
void ResourceManager::CreateTextureSampler(Texture* texture)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(GraphicsSystem::GetPhysicalDevice(), &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(texture->mipLevels);

    if(vkCreateSampler(GraphicsSystem::GetDevice(), &samplerInfo, nullptr, &texture->textureSampler) != VK_SUCCESS)
        throw std::runtime_error("Failed to create texture sampler!");
}
VkImageView ResourceManager::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if(vkCreateImageView(GraphicsSystem::GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("Failed to create texture image view!");

    return imageView;
}

std::unordered_map<std::wstring, std::wstring> ResourceManager::GetAllFilesInDirectory(std::wstring directory, std::vector<std::wstring> extensions)
{
    std::unordered_map<std::wstring, std::wstring> identifiers;

    for(const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
    {
        if(std::filesystem::is_regular_file(entry))
        {
            std::wstring filePath = entry.path();
            size_t directoryEnd = filePath.find(directory) + directory.length();
            size_t extensionStart = filePath.find_last_of('.');

            std::wstring localPath = filePath.substr(directoryEnd);
            std::wstring identifier = filePath.substr(directoryEnd, extensionStart - directoryEnd);
            std::wstring extension = filePath.substr(extensionStart);

            if(std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
                identifiers.emplace(localPath, identifier);

            /*for(char c : identifier)
                std::cout << c;
            std::cout << std::endl;*/
        }
    }

    return identifiers;
}

std::string ResourceManager::GetExePath()
{
    // Assume the path is just the "current directory" for now
    std::string path = ".\\";

    // Get the real, full path to this executable
    char currentDir[1024] = {};
    GetModuleFileNameA(0, currentDir, 1024);

    // Find the location of the last slash charaacter
    char* lastSlash = strrchr(currentDir, '\\');
    if (lastSlash)
    {
        // End the string at the last slash character, essentially
        // chopping off the exe's file name.  Remember, c-strings
        // are null-terminated, so putting a "zero" character in 
        // there simply denotes the end of the string.
        *lastSlash = 0;

        // Set the remainder as the path
        path = currentDir;
    }

    // Toss back whatever we've found
    return path;
}

std::string ResourceManager::FixPath(const std::string& relativeFilePath)
{
    return GetExePath() + "\\" + relativeFilePath;
}
std::wstring ResourceManager::FixPath(const std::wstring& relativeFilePath)
{
    return NarrowToWide(GetExePath()) + L"\\" + relativeFilePath;
}
std::string ResourceManager::WideToNarrow(const std::wstring& str)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), 0, 0, 0, 0);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &result[0], size, 0, 0);
    return result;
}
std::wstring ResourceManager::NarrowToWide(const std::string& str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), 0, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}