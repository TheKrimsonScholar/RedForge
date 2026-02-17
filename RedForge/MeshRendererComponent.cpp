#include "MeshRendererComponent.h"

#include "ResourceManager.h"

#include "Assets.h"

void MeshRendererComponent::UseMeshDefaults(const Assets& assets, const std::wstring& meshIdentifier)
{
	mesh = MeshRef(meshIdentifier);
	material = MaterialRef(assets.GetMesh(meshIdentifier).defaultMaterial->identifier);
}