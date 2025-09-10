#pragma once

#include <string>

#include "MeshRef.h"
#include "MaterialRef.h"

#include "TransformComponent.h"

#include "ComponentRegistrationMacros.h"

#include "Exports.h"

REDFORGE_API struct MeshRendererComponent
{
	MeshRef mesh;
	MaterialRef material;

	uint32_t rendererIndex;

public:
	REDFORGE_API void UseMeshDefaults(const std::wstring& meshIdentifier);
};

REGISTER_COMPONENT_BEGIN(MeshRendererComponent)
COMPONENT_REQUIRES(TransformComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VAR(MeshRef, mesh)
COMPONENT_VAR(MaterialRef, material)
COMPONENT_VARS_END
REGISTER_COMPONENT_END(MeshRendererComponent)