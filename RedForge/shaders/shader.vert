#version 450

struct InstanceData
{
    // Stable index used for accessing all per-object data.
    uint rendererIndex;

    //uint transformIndex;
    uint meshIndex;
    uint materialIndex;
};

layout(set = 0, binding = 1) readonly buffer Transforms
{
    mat4 transforms[];
};
layout(set = 0, binding = 2) readonly buffer Instances
{
    InstanceData instances[];
};
layout(set = 0, binding = 4) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
} cameraUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out flat uint instanceID;

// Push constant or instanced attribute
//layout(location = 3) in uint objectIndex;

void main()
{
    InstanceData instanceData = instances[gl_InstanceIndex];

    gl_Position = cameraUBO.proj * cameraUBO.view * transforms[instanceData.rendererIndex] * vec4(inPosition, 1.0);

    fragColor = inColor;
    fragUV = inUV;
    instanceID = gl_InstanceIndex;
}