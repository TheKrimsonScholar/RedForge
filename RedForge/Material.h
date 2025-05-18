#pragma once

#include <vector>

#include "Texture.h"

class Material
{
public:
	Texture* texture;

	VkDescriptorSet descriptorSets[2];

	// Index of the material in the global materials array, assigned by the graphics system.
	uint32_t index = -1;
};