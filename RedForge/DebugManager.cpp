#include "DebugManager.h"

#include <iostream>

#include "ResourceManager.h"
#include "TimeManager.h"

#include "DebugLogEvent.h"

#include "SystemRegistrationMacros.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

REGISTER_SYSTEM_BEGIN(DebugManager)
SYSTEM_REQUIRES(ResourceManager)
REGISTER_SYSTEM_END(DebugManager)

void DebugManager::Startup(const EngineStartupParams& params, World& world)
{
	Instance = this;

	coutBuffer = std::cout.rdbuf();

	// Redirect cout to the custom stream
	std::cout.rdbuf(coutStream.rdbuf());

	Assets& assets = world.GetResource<Assets>();

	SetDebugMaterial(&assets.GetMaterial(L"default"));

	SetDebugBoxMesh(&assets.GetMesh(L"primitives\\cube.obj"));
	SetDebugSphereMesh(&assets.GetMesh(L"primitives\\sphere.obj"));
}
void DebugManager::PostStartup(const EngineStartupParams& params, World& world)
{

}
void DebugManager::Shutdown(const EngineShutdownParams& params, World& world)
{
	// Restore original cout buffer
	std::cout.rdbuf(coutBuffer);
}

void DebugManager::PrintLogMessage(const DebugLogEvent& logEvent)
{
	Instance->debugLog.push_back(logEvent);

	Instance->onLogMessagePrinted.Broadcast(logEvent);
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

void DebugManager::Update(LocalSystemContext& ctx, float deltaTime)
{
	coutStream.clear(); // Clear edit-only flags
	coutStream.seekg(lastCoutReadPosition); // Skip to last complete line read; read forward from here

	std::string line;
	while(std::getline(coutStream, line))
	{
		// If the line is complete (returns before end-of-file), add line to log
		if(!coutStream.eof())
		{
			DebugLogEvent logEvent;
			logEvent.logType = ELogType::Cout;
			logEvent.text = line;

			PrintLogMessage(logEvent);
			lastCoutReadPosition = coutStream.tellg();
		}
	}

	const std::vector<DebugLogEvent>& logEvents = ctx.PollEvents<DebugLogEvent>();
	for(const DebugLogEvent& logEvent : logEvents)
		PrintLogMessage(logEvent);
}

void DebugManager::UpdateAllWireframes(float deltaTime)
{
	std::vector<DebugEntity> expiredEntities;

	// Decrement all lifetimes and remove expired entities
	for(auto& debugEntity : Instance->debugEntities)
	{
		debugEntity.second -= deltaTime;
		if(debugEntity.second <= 0.0f)
			expiredEntities.push_back(debugEntity.first);
	}
	for(DebugEntity e : expiredEntities)
		Instance->debugEntities.erase(e);
}