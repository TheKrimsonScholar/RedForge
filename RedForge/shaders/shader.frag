#version 450
#extension GL_EXT_nonuniform_qualifier : require

struct MaterialData
{
    uint textureIndex;
};

struct InstanceData
{
    // Stable index used for accessing all per-object data.
    uint rendererIndex;

    //uint transformIndex;
    uint meshIndex;
    uint materialIndex;
};

layout(set = 0, binding = 0) readonly buffer Materials
{
    MaterialData materials[];
};
layout(set = 0, binding = 2) readonly buffer Instances
{
    InstanceData instances[];
};
layout(set = 0, binding = 4) uniform sampler2D Textures[];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in flat uint instanceID;

layout(location = 0) out vec4 outColor;

void main()
{
    InstanceData instanceData = instances[instanceID];
    MaterialData materialData = materials[instanceData.materialIndex];

    outColor = vec4(fragColor * texture(Textures[nonuniformEXT(materialData.textureIndex)], fragUV).rgb, 1.0);
}