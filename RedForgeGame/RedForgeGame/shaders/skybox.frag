#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 3) uniform samplerCube SkyboxTextureCube;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in flat uint instanceID;
layout(location = 3) in vec3 sampleDirection;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(SkyboxTextureCube, sampleDirection);
}