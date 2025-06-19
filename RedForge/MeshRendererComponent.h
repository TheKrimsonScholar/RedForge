#pragma once

#include <string>

#include "Mesh.h"
#include "Material.h"

#include "ComponentMacros.h"

#include "Exports.h"

REDFORGE_API struct MeshRendererComponent
{
	Mesh* mesh;
	Material* material;

	uint32_t rendererIndex;

public:
	REDFORGE_API void UseMeshDefaults(const std::wstring& meshIdentifier);
};

REGISTER_COMPONENT_BEGIN(MeshRendererComponent)
COMPONENT_VAR(Mesh*, mesh)
COMPONENT_VAR(Material*, material)
REGISTER_COMPONENT_END(MeshRendererComponent)