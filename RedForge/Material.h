#pragma once

#include "Asset.h"

#include <vector>
#include <string>

#include "Texture.h"

#include "Exports.h"

class Material : public IAsset
{
public:
	const Texture* albedoTexture = nullptr;
	const Texture* normalsTexture = nullptr;
	const Texture* roughnessTexture = nullptr;
	const Texture* metalnessTexture = nullptr;

	VkDescriptorSet descriptorSets[2];

	std::wstring identifier;
	// Index of the material in the global materials array, assigned by the graphics system.
	uint32_t index = -1;

private:
	Material();

	virtual void Startup(const EngineStartupParams& params, World& world) override;
	virtual void Shutdown(const EngineShutdownParams& params, World& world) override;

	friend struct Assets;
};