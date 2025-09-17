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
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 fragLocation;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragTangent;
layout(location = 3) out vec2 fragUV;
layout(location = 4) out flat uint instanceID;

void main()
{
    InstanceData instanceData = instances[gl_InstanceIndex];

    gl_Position = cameraUBO.proj * cameraUBO.view * transforms[instanceData.rendererIndex] * vec4(inPosition, 1.0);

    fragLocation = vec3(transforms[instanceData.rendererIndex] * vec4(inPosition, 1.0));
    fragNormal = normalize(mat3x3(transpose(inverse(transforms[instanceData.rendererIndex]))) * inNormal);
    fragTangent = normalize(mat3x3(transforms[instanceData.rendererIndex]) * inTangent);
    fragUV = inUV;
    instanceID = gl_InstanceIndex;
}