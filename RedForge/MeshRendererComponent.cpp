#include "MeshRendererComponent.h"

#include "ResourceManager.h"

void MeshRendererComponent::UseMeshDefaults(const std::wstring& meshIdentifier)
{
	mesh = MeshRef(meshIdentifier);
	material = MaterialRef(ResourceManager::GetMesh(meshIdentifier)->defaultMaterial->identifier);
}