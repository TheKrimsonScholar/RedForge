#pragma once

#include <array>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;

    bool operator==(const Vertex& other) const;

    static VkVertexInputBindingDescription GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
};

namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(const Vertex& v) const
        {
            return hash<float>()(v.pos.x) ^ hash<float>()(v.pos.y) ^ hash<float>()(v.pos.z) ^ 
                hash<float>()(v.color.r) ^ hash<float>()(v.color.g) ^ hash<float>()(v.color.b) ^ 
                hash<float>()(v.uv.x) ^ hash<float>()(v.uv.y);
        }
    };
}