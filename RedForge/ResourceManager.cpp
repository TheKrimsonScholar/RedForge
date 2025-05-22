#include "ResourceManager.h"

#include <filesystem>
#include <unordered_map>

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

void ResourceManager::AddMaterial(Texture* texture)
{
	Material* material = new Material();
	material->texture = texture;

	material->index = Instance->materials.size();
	Instance->materials.push_back(material);
}

Texture* ResourceManager::LoadTexture(const std::wstring& filePath)
{
	Texture* texture = new Texture();

	CreateTextureImage(texture, filePath);

    texture->textureImageView = CreateImageView(texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture->mipLevels);

	CreateTextureSampler(texture);

	texture->index = Instance->textures.size();
	Instance->textures.push_back(texture);

	return texture;
}
Mesh* ResourceManager::LoadModel(const std::wstring& filePath)
{
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

    GraphicsSystem::CreateVertexBuffer(mesh->vertices, mesh->vertexBuffer, mesh->vertexBufferMemory);
    GraphicsSystem::CreateIndexBuffer(mesh->indices, mesh->indexBuffer, mesh->indexBufferMemory);

	mesh->index = Instance->meshes.size();
    Instance->meshes.push_back(mesh);

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