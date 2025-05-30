#pragma once

#include <array>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 uv;

    bool operator==(const Vertex& other) const;

    static VkVertexInputBindingDescription GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
};
namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(const Vertex& v) const
        {
            return hash<float>()(v.pos.x) ^ hash<float>()(v.pos.y) ^ hash<float>()(v.pos.z) ^ 
                hash<float>()(v.normal.x) ^ hash<float>()(v.normal.y) ^ hash<float>()(v.normal.z) ^
                hash<float>()(v.tangent.x) ^ hash<float>()(v.tangent.y) ^ hash<float>()(v.tangent.z) ^
                hash<float>()(v.uv.x) ^ hash<float>()(v.uv.y);
        }
    };
}