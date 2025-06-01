#pragma once

#include <string>

#include "Mesh.h"
#include "Material.h"

#include "Exports.h"

REDFORGE_API struct MeshRendererComponent
{
	Mesh* mesh;
	Material* material;

	uint32_t rendererIndex;

public:
	REDFORGE_API void UseMeshDefaults(const std::wstring& meshIdentifier);
};