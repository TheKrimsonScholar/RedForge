#include "MeshRendererComponent.h"

#include "ResourceManager.h"

void MeshRendererComponent::UseMeshDefaults(const std::wstring& meshIdentifier)
{
	mesh = ResourceManager::GetMesh(meshIdentifier);
	material = mesh->defaultMaterial;
}