#version 450
#extension GL_EXT_nonuniform_qualifier : require

const float F0_NON_METAL = 0.04; // Constant Fresnel value for non-metals (glass and plastic have values of about 0.04)
const float MIN_ROUGHNESS = 0.0000001;

const float PI = 3.14159265359;

const uint LIGHT_TYPE_DIRECTIONAL = 0;
const uint LIGHT_TYPE_POINT = 1;

struct MaterialData
{
    uint albedoIndex;
    uint normalsIndex;
    uint roughnessIndex;
    uint metalnessIndex;
};

struct InstanceData
{
    // Stable index used for accessing all per-object data.
    uint rendererIndex;

    //uint transformIndex;
    uint meshIndex;
    uint materialIndex;
};

struct LightData
{
    vec3 direction;
    uint lightType;
    vec3 color;
    float intensity;
    vec3 location;
    float range;
    float spotInnerAngle;
    float spotOuterAngle;

    float _PADDING[2];
};

layout(set = 0, binding = 0) readonly buffer Materials
{
    MaterialData materials[];
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
layout(set = 0, binding = 5) readonly buffer LightsBuffer
{
    uint lightCount;
    float _padding[3];
    //vec3 _padding;
    LightData lights[];
};
layout(set = 0, binding = 6) uniform sampler2D Textures[];

layout(location = 0) in vec3 fragLocation;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragTangent;
layout(location = 3) in vec2 fragUV;
layout(location = 4) in flat uint instanceID;

layout(location = 0) out vec4 outColor;

float attenuate(LightData light, vec3 worldPosition)
{
    float dist = distance(light.location, worldPosition);
    float attenuated = clamp(1.0f - (dist * dist / (light.range * light.range)), 0, 1);
    
    return attenuated * attenuated;
}

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(vec3 normal, vec3 dirToLight)
{
    return clamp(dot(normal, dirToLight), 0, 1);
}

vec3 DiffuseEnergyConserve(vec3 diffuse, vec3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}

float D_GGX(vec3 n, vec3 h, float roughness)
{
	// Pre-calculations
    float NdotH = clamp(dot(n, h), 0, 1);
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	// Can go to zero if roughness is 0 and NdotH is 1
    float denomToSquare = NdotH2 * (a2 - 1) + 1;

	// Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}
vec3 F_Schlick(vec3 v, vec3 h, vec3 f0)
{
	// Pre-calculations
    float VdotH = clamp(dot(v, h), 0, 1);

	// Final value
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}
float G_SchlickGGX(vec3 n, vec3 v, float roughness)
{
	// End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = clamp(dot(n, v), 0, 1);

	// Final value
	// Note: Numerator should be NdotV (or NdotL depending on parameters).
	// However, these are also in the BRDF's denominator, so they'll cancel!
	// We're leaving them out here AND in the BRDF function as the
	// dot products can get VERY small and cause rounding errors.
    return 1 / (NdotV * (1 - k) + k);
}
vec3 MicrofacetBRDF(vec3 n, vec3 l, vec3 v, float roughness, vec3 f0, out vec3 F_out)
{
    // Other vectors
    vec3 h = normalize(v + l);

	// Run numerator functions
    float D = D_GGX(n, h, roughness);
    vec3 F = F_Schlick(v, h, f0);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
	
	// Pass F out of the function for diffuse balance
    F_out = F;

    vec3 specularResult = (D * F * G) / 4;

    return specularResult * max(dot(n, l), 0);
}

vec3 DirectionalLightPBR(LightData light, vec3 normal, vec3 worldPosition, vec3 cameraLocation, float roughness, float metalness, vec3 albedoColor, vec3 specularColor)
{
    // Diffuse
    vec3 diffuseTerm = DiffusePBR(normal, normalize(-light.direction)).xxx;

	// Specular
    vec3 F;
    vec3 specularTerm = MicrofacetBRDF(normal, normalize(-light.direction), normalize(cameraLocation - worldPosition), roughness, specularColor, F);
        
    diffuseTerm = DiffuseEnergyConserve(diffuseTerm, F, metalness);
    
    return albedoColor * (diffuseTerm + specularTerm) * light.intensity * light.color;
}
vec3 PointLightPBR(LightData light, vec3 normal, vec3 worldPosition, vec3 cameraLocation, float roughness, float metalness, vec3 albedoColor, vec3 specularColor)
{
    // Diffuse
    vec3 diffuseTerm = DiffusePBR(normal, normalize(light.location - worldPosition)).xxx;

	// Specular
    vec3 F;
    vec3 specularTerm = MicrofacetBRDF(normal, normalize(light.location - worldPosition), normalize(cameraLocation - worldPosition), roughness, specularColor, F);
    specularTerm *= attenuate(light, worldPosition); // Attenuate specular term
        
    // Energy conservation: scale diffusion amount to be less with a greater specular amounts
    diffuseTerm = DiffuseEnergyConserve(diffuseTerm, F, metalness);
    diffuseTerm *= attenuate(light, worldPosition); // Attenuate diffuse term
    
    return albedoColor * (diffuseTerm + specularTerm) * light.intensity * light.color;
}

void main()
{
    InstanceData instanceData = instances[instanceID];
    MaterialData materialData = materials[instanceData.materialIndex];

    vec3 albedoColor = texture(Textures[nonuniformEXT(materialData.albedoIndex)], fragUV).rgb;
    vec3 mappedNormal = normalize(texture(Textures[nonuniformEXT(materialData.normalsIndex)], fragUV).rgb * 2 - vec3(1, 1, 1));
    float roughness = texture(Textures[nonuniformEXT(materialData.roughnessIndex)], fragUV).r;
    float metalness = texture(Textures[nonuniformEXT(materialData.metalnessIndex)], fragUV).r;
    vec3 specularColor = mix(F0_NON_METAL.xxx, albedoColor, metalness);

    // Calculate normal, tangent and bitangent for normal mapping
    // Orthonormalize normal and tangent using Gram-Schmidt Process
    vec3 N = normalize(fragNormal);
    vec3 T = normalize(fragTangent - N * dot(N, fragTangent));
    vec3 B = cross(T, N);
    mat3x3 rotationMatrix = mat3x3(T, B, N); // Rotation matrix that transforms tangent space to world space

    mappedNormal = normalize(mappedNormal * rotationMatrix);

    vec3 totalColor = vec3(0);
    for(uint i = 0; i < lightCount; i++)
    {
        switch(lights[i].lightType)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalColor += DirectionalLightPBR(lights[i], fragNormal, fragLocation, vec3(cameraUBO.view[3]), roughness, metalness, albedoColor, specularColor);
                break;
            case LIGHT_TYPE_POINT:
                totalColor += PointLightPBR(lights[i], fragNormal, fragLocation, vec3(cameraUBO.view[3]), roughness, metalness, albedoColor, specularColor);
                break;
            default: break;
        }
    }

    outColor = vec4(totalColor, 1);
}