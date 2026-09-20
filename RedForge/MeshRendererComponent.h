#pragma once

#include <string>

#include "MeshRef.h"
#include "MaterialRef.h"

#include "TransformComponent.h"

#include "ComponentRegistration.h"

#include "Exports.h"

struct Assets;

REDFORGE_API struct MeshRendererComponent : public IComponent
{
	SERIALIZED_COMPONENT_VARS()
	REPLICATED_COMPONENT_VARS()

	MeshRef mesh;
	MaterialRef material;

	uint32_t rendererIndex;

public:
	REDFORGE_API void UseMeshDefaults(const Assets& assets, const std::wstring& meshIdentifier);
};

REGISTER_COMPONENT_TYPE_BEGIN(MeshRendererComponent, IComponent)
COMPONENT_REQUIRES(TransformComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VAR(MeshRef, mesh)
COMPONENT_VAR(MaterialRef, material)
COMPONENT_VARS_END
REGISTER_COMPONENT_TYPE_END(MeshRendererComponent)