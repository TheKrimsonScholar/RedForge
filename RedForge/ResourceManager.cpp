#include "ResourceManager.h"

#include <unordered_map>
#include <iostream>

#include "rapidobj/rapidobj.hpp"

#include "GraphicsSystem.h"
#include "PathUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

std::filesystem::path GetEngineTexturesPath()
{
    return GetEngineAssetsPath().append(L"textures/");
}
std::filesystem::path GetEngineTextureCubesPath()
{
    return GetEngineAssetsPath().append(L"textures/textureCubes/");
}
std::filesystem::path GetEngineMaterialsPath()
{
    return GetEngineAssetsPath().append(L"materials/");
}
std::filesystem::path GetEngineMeshesPath()
{
    return GetEngineAssetsPath().append(L"meshes/");
}

void ResourceManager::Startup(const EngineStartupParams& params, World& world)
{
    Instance = this;

	Assets& assets = world.GetResource<Assets>();
	GraphicsState& graphicsState = world.GetResource<GraphicsState>();

    ResourceManager::LoadAllTextures(assets, graphicsState);

    //LoadTextureCube(L"textures/textureCubes/Cold Sunset/.png");
    //LoadTextureCube(L"textures/textureCubes/Planet/.png");
    LoadTextureCube(assets, graphicsState, GetEngineTextureCubesPath().append(L"Clouds Blue/.png"));
    graphicsState.skyboxTextureCube = &assets.GetTextureCube(L"Clouds Blue/.png");

    //LoadAllMaterials();
    LoadAllMeshes(assets, graphicsState);

    Material* material = &assets.GetMaterial(L"default");
    material->albedoTexture = &assets.GetTexture(L"scratched_albedo.png");
    material->normalsTexture = &assets.GetTexture(L"scratched_normals.png");
    material->roughnessTexture = &assets.GetTexture(L"scratched_roughness.png");
    material->metalnessTexture = &assets.GetTexture(L"scratched_metal.png");
}
void ResourceManager::PostStartup(const EngineStartupParams& params, World& world)
{

}
void ResourceManager::Shutdown(const EngineShutdownParams& params, World& world)
{
    
}

void ResourceManager::Update(SystemContext<>& ctx, float deltaTime)
{

}

void ResourceManager::LoadAllTextures(Assets& assets, const GraphicsState& graphicsState)
{
    const std::vector<std::wstring> VALID_TYPES = { L".png" };
    std::vector<std::filesystem::path> texturePaths = File::GetAllFilesInDirectory(GetEngineTexturesPath(), VALID_TYPES);
    
    // Load all textures from their paths and associate them with identifiers in an unordered map
	for(auto& texturePath : texturePaths)
        LoadTexture(assets, graphicsState, GetEngineTexturesPath().append(texturePath.wstring()));
}
void ResourceManager::LoadAllMaterials(Assets& assets, const GraphicsState& graphicsState)
{
	const std::vector<std::wstring> VALID_TYPES = { L".mtl" };
    std::vector<std::filesystem::path> materialPaths = File::GetAllFilesInDirectory(GetEngineMaterialsPath(), VALID_TYPES);

	// Load all materials from their paths and associate them with identifiers in an unordered map
	for(auto& materialPath : materialPaths)
		LoadTexture(assets, graphicsState, GetEngineMaterialsPath().append(materialPath.wstring()));
}
void ResourceManager::LoadAllMeshes(Assets& assets, const GraphicsState& graphicsState)
{
    const std::vector<std::wstring> VALID_TYPES = { L".obj" };
    std::vector<std::filesystem::path> meshPaths = File::GetAllFilesInDirectory(GetEngineMeshesPath(), VALID_TYPES);

    for(auto& meshPath : meshPaths)
        LoadModel(assets, graphicsState, GetEngineMeshesPath().append(meshPath.wstring()));
}

const Texture& ResourceManager::LoadTexture(Assets& assets, const GraphicsState& graphicsState, const std::filesystem::path& filePath)
{
	std::wstring identifier = filePath.wstring().substr(GetEngineTexturesPath().wstring().length());

	// Check if the texture is already loaded
	if(assets.HasTexture(identifier))
		return assets.GetTexture(identifier);

	Texture& texture = assets.CreateTexture(identifier);

	CreateTextureImage(graphicsState, texture, filePath);

    texture.textureImageView = CreateImageView(graphicsState, texture.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture.mipLevels);

	CreateTextureSampler(graphicsState, texture);

	return texture;
}
const TextureCube& ResourceManager::LoadTextureCube(Assets& assets, const GraphicsState& graphicsState, const std::filesystem::path& filePath)
{
    std::wstring identifier = filePath.wstring().substr(GetEngineTextureCubesPath().wstring().length());

    // Check if the texture is already loaded
	if(assets.HasTextureCube(identifier))
		return assets.GetTextureCube(identifier);

	TextureCube& textureCube = assets.CreateTextureCube(identifier);

	std::wstring extension = filePath.wstring().substr(filePath.wstring().find_last_of(L"."));
	std::wstring filePathWithoutExtension = filePath.wstring().substr(0, filePath.wstring().find_last_of(L"."));
	CreateTextureCubeImage(graphicsState, textureCube, 
        {
            filePathWithoutExtension + L"_right" + extension,
            filePathWithoutExtension + L"_left" + extension,
            filePathWithoutExtension + L"_up" + extension,
            filePathWithoutExtension + L"_down" + extension,
            filePathWithoutExtension + L"_forward" + extension,
            filePathWithoutExtension + L"_backward" + extension
        });

    textureCube.textureImageView = 
        CreateImageView(graphicsState, textureCube.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, textureCube.mipLevels, 
            VK_IMAGE_VIEW_TYPE_CUBE, 6);

	CreateTextureCubeSampler(graphicsState, textureCube);

	return textureCube;
}
const Material& ResourceManager::LoadMaterial(Assets& assets, const std::filesystem::path& filePath)
{
    std::wstring identifier = filePath.wstring().substr(GetEngineMaterialsPath().wstring().length());

    // Check if the material is already loaded
	if(assets.HasMaterial(identifier))
		return assets.GetMaterial(identifier);

    Material& material = assets.CreateMaterial(identifier);

	/* --- Custom MAT parsing --- */

    return material;
}
const Mesh& ResourceManager::LoadModel(Assets& assets, const GraphicsState& graphicsState, const std::filesystem::path& filePath)
{
    std::wstring identifier = filePath.wstring().substr(GetEngineMeshesPath().wstring().length());

    // Check if the mesh is already loaded
	if(assets.HasMesh(identifier))
		return assets.GetMesh(identifier);

    Mesh& mesh = assets.CreateMesh(identifier);

    rapidobj::Result obj = rapidobj::ParseFile(filePath.c_str());
    std::string errorMessage = obj.error.code.message();

    std::unordered_map<Vertex, uint32_t> vertexMap;

    // For each shape
    for(rapidobj::Shape& shape : obj.shapes)
    {
        uint32_t firstFaceIndex = 0;

        // For each face
        for(int f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            // Only support triangles and quads
            assert((shape.mesh.num_face_vertices[f] == 3 || shape.mesh.num_face_vertices[f] == 4) && "OBJ file contains a face that is not a triangle or quad!");

            std::vector<Vertex> faceVertices;

            // For each vertex
            for(int j = 0; j < shape.mesh.num_face_vertices[f]; j++)
            {
                rapidobj::Index index = shape.mesh.indices[firstFaceIndex + j];

                Vertex vertex = {};
                vertex.pos = { obj.attributes.positions[index.position_index * 3], obj.attributes.positions[index.position_index * 3 + 1], obj.attributes.positions[index.position_index * 3 + 2] };
                vertex.normal = { obj.attributes.normals[index.normal_index * 3], obj.attributes.normals[index.normal_index * 3 + 1], obj.attributes.normals[index.normal_index * 3 + 2] };
                vertex.tangent = { 0.0f, 0.0f, 0.0f }; // Calculate tangents later
                vertex.uv = { obj.attributes.texcoords[index.texcoord_index * 2], 1.0f - obj.attributes.texcoords[index.texcoord_index * 2 + 1] };

                if(vertexMap.count(vertex) == 0)
                {
                    vertexMap[vertex] = mesh.vertices.size();
                    mesh.vertices.push_back(vertex);
                }

                mesh.indices.push_back(vertexMap[vertex]);
                faceVertices.push_back(vertex);
            }

            // If this face is a quad, complete the second triangle by reusing two indices
            if(shape.mesh.num_face_vertices[f] == 4)
            {
                mesh.indices.push_back(vertexMap[faceVertices[0]]);
                mesh.indices.push_back(vertexMap[faceVertices[2]]);
            }

            firstFaceIndex += shape.mesh.num_face_vertices[f];
        }

        // Second pass through triangles for tangent calculations
        for(int i = 0; i < mesh.indices.size();)
        {
	        // Grab indices and vertices of first triangle
	        unsigned int i1 = mesh.indices[i++];
	        unsigned int i2 = mesh.indices[i++];
	        unsigned int i3 = mesh.indices[i++];
	        Vertex* v1 = &mesh.vertices[i1];
	        Vertex* v2 = &mesh.vertices[i2];
	        Vertex* v3 = &mesh.vertices[i3];
	        // Calculate vectors relative to triangle positions
	        float x1 = v2->pos.x - v1->pos.x;
	        float y1 = v2->pos.y - v1->pos.y;
	        float z1 = v2->pos.z - v1->pos.z;
	        float x2 = v3->pos.x - v1->pos.x;
	        float y2 = v3->pos.y - v1->pos.y;
	        float z2 = v3->pos.z - v1->pos.z;
	        // Do the same for vectors relative to triangle uv's
	        float s1 = v2->uv.x - v1->uv.x;
	        float t1 = v2->uv.y - v1->uv.y;
	        float s2 = v3->uv.x - v1->uv.x;
	        float t2 = v3->uv.y - v1->uv.y;
	        // Create vectors for tangent calculation
	        float r = 1.0f / (s1 * t2 - s2 * t1);
	        float tx = (t2 * x1 - t1 * x2) * r;
	        float ty = (t2 * y1 - t1 * y2) * r;
	        float tz = (t2 * z1 - t1 * z2) * r;
	        // Adjust tangents of each vert of the triangle
	        v1->tangent.x += tx;
	        v1->tangent.y += ty;
	        v1->tangent.z += tz;
	        v2->tangent.x += tx;
	        v2->tangent.y += ty;
	        v2->tangent.z += tz;
	        v3->tangent.x += tx;
	        v3->tangent.y += ty;
	        v3->tangent.z += tz;
        }
        // Ensure all of the tangents are orthogonal to the normals
        for(int i = 0; i < mesh.vertices.size(); i++)
        {
	        // Grab the two vectors
	        glm::vec3 normal = mesh.vertices[i].normal;
	        glm::vec3 tangent = mesh.vertices[i].tangent;
	        // Use Gram-Schmidt orthonormalize to ensure
	        // the normal and tangent are exactly 90 degrees apart
            mesh.vertices[i].tangent = glm::normalize(tangent - normal * glm::dot(normal, tangent));
        }
    }

    // For each material associated with the .obj
    for(rapidobj::Material& mat : obj.materials)
    {
        if(assets.HasMaterial(NarrowToWide(mat.name)))
        {
		    mesh.defaultMaterial = &assets.GetMaterial(NarrowToWide(mat.name));
            continue;
        }

		Material& material = assets.CreateMaterial(NarrowToWide(mat.name));

		material.albedoTexture = &LoadTexture(assets, graphicsState, GetEngineTexturesPath().append(mat.diffuse_texname));

		mesh.defaultMaterial = &material;
    }

    assert(mesh.vertices.size() > 0 && mesh.indices.size() > 0 && "Attempting to load empty mesh file.");
    
    graphicsState.CreateVertexBuffer(mesh.vertices, mesh.vertexBuffer, mesh.vertexBufferMemory);
    graphicsState.CreateIndexBuffer(mesh.indices, mesh.indexBuffer, mesh.indexBufferMemory);

    return mesh;
}

void ResourceManager::CreateTextureImage(const GraphicsState& graphicsState, Texture& texture, const std::filesystem::path& filePath)
{
	int textureWidth, textureHeight, textureChannels;
    stbi_uc* pixels = stbi_load(filePath.string().c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = textureWidth * textureHeight * 4;
    texture.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

    if(!pixels)
        throw std::runtime_error("Failed to load texture image!");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    graphicsState.CreateBuffer(
        imageSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory);

    void* data;
    vkMapMemory(graphicsState.device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(graphicsState.device, stagingBufferMemory);

    stbi_image_free(pixels);

    graphicsState.CreateImage(
        textureWidth, textureHeight, texture.mipLevels, 
        VK_SAMPLE_COUNT_1_BIT, 
        VK_FORMAT_R8G8B8A8_SRGB, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        texture.textureImage, 
        texture.textureImageMemory);

    graphicsState.TransitionImageLayout(texture.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture.mipLevels);
    graphicsState.CopyBufferToImage(stagingBuffer, texture.textureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));

    vkDestroyBuffer(graphicsState.device, stagingBuffer, nullptr);
    vkFreeMemory(graphicsState.device, stagingBufferMemory, nullptr);

    GenerateMipmaps(graphicsState, texture.textureImage, VK_FORMAT_R8G8B8A8_SRGB, textureWidth, textureHeight, texture.mipLevels);
}
void ResourceManager::CreateTextureCubeImage(const GraphicsState& graphicsState, TextureCube& textureCube, const std::array<std::filesystem::path, 6>& filePaths)
{
    int textureWidth, textureHeight, textureChannels;
    VkDeviceSize imageSize = 0;
    stbi_uc* pixels[6];

    for(int i = 0; i < 6; i++)
    {
        pixels[i] = stbi_load(filePaths[i].string().c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
        if(!pixels[i])
            throw std::runtime_error("Failed to load texture image for cube face " + std::to_string(i) + "!");
        
        assert((imageSize == 0 || imageSize == textureWidth * textureHeight * 4) && "Texture cube faces are not of equal dimension.");
        imageSize = textureWidth * textureHeight * 4;
    }
    
    // Don't use mipmaps for cubemaps
    textureCube.mipLevels = 1;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    graphicsState.CreateBuffer(
        imageSize * 6, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory);

    void* data;
    vkMapMemory(graphicsState.device, stagingBufferMemory, 0, imageSize * 6, 0, &data);
    for(int i = 0; i < 6; i++)
    {
        memcpy(static_cast<uint8_t*>(data) + i * imageSize, pixels[i], static_cast<size_t>(imageSize));
        stbi_image_free(pixels[i]);
    }
    vkUnmapMemory(graphicsState.device, stagingBufferMemory);

    graphicsState.CreateImage(
        textureWidth, textureHeight, textureCube.mipLevels,
        VK_SAMPLE_COUNT_1_BIT, 
        VK_FORMAT_R8G8B8A8_SRGB, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        textureCube.textureImage, 
        textureCube.textureImageMemory, 
        VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
        6);

    graphicsState.TransitionImageLayout(textureCube.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureCube.mipLevels, 6);
    graphicsState.CopyBufferToImage(stagingBuffer, textureCube.textureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight), 6);
    graphicsState.TransitionImageLayout(textureCube.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureCube.mipLevels, 6);

    vkDestroyBuffer(graphicsState.device, stagingBuffer, nullptr);
    vkFreeMemory(graphicsState.device, stagingBufferMemory, nullptr);
}
void ResourceManager::GenerateMipmaps(const GraphicsState& graphicsState, VkImage image, VkFormat imageFormat, int32_t textureWidth, int32_t textureHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(graphicsState.physicalDevice, imageFormat, &formatProperties);

    if(!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        throw std::runtime_error("Texture image format does not support linear blitting!");

    VkCommandBuffer commandBuffer = graphicsState.BeginSingleTimeCommands();

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
    
    graphicsState.EndSingleTimeCommands(commandBuffer);
}
void ResourceManager::CreateTextureSampler(const GraphicsState& graphicsState, Texture& texture)
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
    vkGetPhysicalDeviceProperties(graphicsState.physicalDevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(texture.mipLevels);

    if(vkCreateSampler(graphicsState.device, &samplerInfo, nullptr, &texture.textureSampler) != VK_SUCCESS)
        throw std::runtime_error("Failed to create texture sampler!");
}
void ResourceManager::CreateTextureCubeSampler(const GraphicsState& graphicsState, TextureCube& textureCube)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(graphicsState.physicalDevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(textureCube.mipLevels);

    if(vkCreateSampler(graphicsState.device, &samplerInfo, nullptr, &textureCube.sampler) != VK_SUCCESS)
        throw std::runtime_error("Failed to create texture cube sampler!");
}
VkImageView ResourceManager::CreateImageView(const GraphicsState& graphicsState, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
    VkImageViewType viewType, uint32_t layerCount)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layerCount;

    VkImageView imageView;
    if(vkCreateImageView(graphicsState.device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("Failed to create texture image view!");

    return imageView;
}