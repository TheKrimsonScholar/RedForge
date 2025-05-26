#include "DebugManager.h"

#include "ResourceManager.h"
#include "TimeManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void DebugManager::Startup()
{
	Instance = this;

	SetDebugMaterial(ResourceManager::GetMaterial(L"default"));

	SetDebugBoxMesh(ResourceManager::GetMesh(L"primitives\\cube.obj"));
	SetDebugSphereMesh(ResourceManager::GetMesh(L"primitives\\sphere.obj"));
}
void DebugManager::Shutdown()
{

}

void DebugManager::DrawDebugBox(glm::vec3 location, glm::quat rotation, glm::vec3 scale, glm::vec4 color, float duration)
{
	DebugEntity debugEntity{};
	debugEntity.mesh = Instance->debugBoxMesh;
	debugEntity.transformMatrix = glm::translate(glm::mat4(1.0f), location) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);

	Instance->debugEntities.emplace(debugEntity, duration);
}
void DebugManager::DrawDebugSphere(glm::vec3 location, glm::quat rotation, glm::vec3 scale, glm::vec4 color, float duration)
{
	DebugEntity debugEntity{};
	debugEntity.mesh = Instance->debugSphereMesh;
	debugEntity.transformMatrix = glm::translate(glm::mat4(1.0f), location) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);

	Instance->debugEntities.emplace(debugEntity, duration);
}

void DebugManager::UpdateAllWireframes()
{
	std::vector<DebugEntity> expiredEntities;

	// Decrement all lifetimes and remove expired entities
	for(auto& debugEntity : Instance->debugEntities)
	{
		debugEntity.second -= TimeManager::GetDeltaTime();
		if(debugEntity.second <= 0.0f)
			expiredEntities.push_back(debugEntity.first);
	}
	for(DebugEntity e : expiredEntities)
		Instance->debugEntities.erase(e);
}