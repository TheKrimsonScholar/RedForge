#pragma once

#include <string>

#include "Mesh.h"
#include "Material.h"

struct MeshRendererComponent
{
	Mesh* mesh;
	Material* material;

	uint32_t rendererIndex;

public:
	void UseMeshDefaults(const std::wstring& meshIdentifier);
};