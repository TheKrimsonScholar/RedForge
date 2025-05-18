#pragma once

#include "Mesh.h"
#include "Material.h"

struct InstanceData;

struct MeshRendererComponent
{
	Mesh* mesh;
	Material* material;

	uint32_t rendererIndex;
};