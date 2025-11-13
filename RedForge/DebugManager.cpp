#include "DebugManager.h"

#include <iostream>

#include "ResourceManager.h"
#include "TimeManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

std::string LogMessage::ToString() const
{
	std::string logTypeString = "UNKNOWN";
	switch(logType)
	{
		case LogType::Cout:
			logTypeString = "COUT";
			break;
		case LogType::Engine:
			logTypeString = "ENGINE";
			break;
		case LogType::Editor:
			logTypeString = "EDITOR";
			break;
		case LogType::Game:
			logTypeString = "GAME";
			break;
		default: break;
	}

	return std::format("[{}] {}", logTypeString, text);
}

void DebugManager::Startup()
{
	Instance = this;

	coutBuffer = std::cout.rdbuf();

	// Redirect cout to the custom stream
	std::cout.rdbuf(coutStream.rdbuf());

	SetDebugMaterial(ResourceManager::GetMaterial(L"default"));

	SetDebugBoxMesh(ResourceManager::GetMesh(L"primitives\\cube.obj"));
	SetDebugSphereMesh(ResourceManager::GetMesh(L"primitives\\sphere.obj"));
}
void DebugManager::Shutdown()
{
	// Restore original cout buffer
	std::cout.rdbuf(coutBuffer);
}

void DebugManager::PrintLogMessage(LogType logType, const std::string& text)
{
	LogMessage message;
	message.timestamp = TimeManager::GetCurrentTime();
	message.logType = logType;
	message.text = text;

	Instance->debugLog.push_back(message);

	Instance->onLogMessagePrinted.Broadcast(message);
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

void DebugManager::Update()
{
	coutStream.clear(); // Clear edit-only flags
	coutStream.seekg(lastCoutReadPosition); // Skip to last complete line read; read forward from here

	std::string line;
	while(std::getline(coutStream, line))
	{
		// If the line is complete (returns before end-of-file), add line to log
		if(!coutStream.eof())
		{
			PrintLogMessage(LogType::Cout, line);
			lastCoutReadPosition = coutStream.tellg();
		}
	}
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