#pragma once

#include <vector>
#include <string>

#include "Texture.h"

class Material
{
public:
	Texture* albedoTexture;
	Texture* normalsTexture;
	Texture* roughnessTexture;
	Texture* metalnessTexture;

	VkDescriptorSet descriptorSets[2];

	std::wstring identifier;
	// Index of the material in the global materials array, assigned by the graphics system.
	uint32_t index = -1;
};