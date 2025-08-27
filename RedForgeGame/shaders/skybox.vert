#version 450

layout(set = 0, binding = 4) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
} cameraUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out flat uint instanceID;
layout(location = 3) out vec3 sampleDirection;

void main()
{
    mat4 viewNoTranslation = cameraUBO.view;
	viewNoTranslation[3][0] = 0;
	viewNoTranslation[3][1] = 0;
	viewNoTranslation[3][2] = 0;

	gl_Position = cameraUBO.proj * (viewNoTranslation * vec4(inPosition, 1));
	gl_Position.z = gl_Position.w; // Ensure depth is 1.0 (z/w = 1.0)

    fragColor = inNormal;
    fragUV = inUV;
    instanceID = gl_InstanceIndex;
	sampleDirection = inPosition;
}