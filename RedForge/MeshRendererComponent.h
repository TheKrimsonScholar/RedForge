#pragma once

#include "Mesh.h"
#include "Material.h"

struct MeshRendererComponent
{
	Mesh* mesh;
	Material* material;

	uint32_t rendererIndex;
};