#pragma once

#include "System.h"

#include <unordered_map>

#include "DebugState.h"

#include "DebugLogEvent.h"

#include "Mesh.h"
#include "Event.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Exports.h"

struct DebugEntity
{
	Mesh* mesh;
	glm::mat4 transformMatrix;

	bool operator==(const DebugEntity& other) const
	{
		return mesh == other.mesh && transformMatrix == other.transformMatrix;
	}
};
namespace std
{
	template<>
	struct hash<DebugEntity>
	{
		size_t operator()(const DebugEntity& entity) const
		{
			return 
				std::hash<Mesh*>()(entity.mesh) 
				^ std::hash<float>()(entity.transformMatrix[0].x) ^ std::hash<float>()(entity.transformMatrix[0].y) ^ std::hash<float>()(entity.transformMatrix[0].z) ^ std::hash<float>()(entity.transformMatrix[0].w)
				^ std::hash<float>()(entity.transformMatrix[1].x) ^ std::hash<float>()(entity.transformMatrix[1].y) ^ std::hash<float>()(entity.transformMatrix[1].z) ^ std::hash<float>()(entity.transformMatrix[1].w)
				^ std::hash<float>()(entity.transformMatrix[2].x) ^ std::hash<float>()(entity.transformMatrix[2].y) ^ std::hash<float>()(entity.transformMatrix[2].z) ^ std::hash<float>()(entity.transformMatrix[2].w)
				^ std::hash<float>()(entity.transformMatrix[3].x) ^ std::hash<float>()(entity.transformMatrix[3].y) ^ std::hash<float>()(entity.transformMatrix[3].z) ^ std::hash<float>()(entity.transformMatrix[3].w);
		}
	};
}

class DebugManager : public System<const DebugState>
{
private:
	static inline DebugManager* Instance;

	Event<const DebugLogEvent&> onLogMessagePrinted;

	std::streambuf* coutBuffer = nullptr;
	std::stringstream coutStream;
	// The ending stream position of the last complete line read from cout.
	size_t lastCoutReadPosition = 0;

	Material* debugMaterial;

	Mesh* debugBoxMesh;
	Mesh* debugSphereMesh;

	std::vector<DebugLogEvent> debugLog;
	std::unordered_map<DebugEntity, float> debugEntities;

public:
	DebugManager() {};
	~DebugManager() {};

	void Startup(const EngineStartupParams& params, World& world) override;
	void PostStartup(const EngineStartupParams& params, World& world) override;
	void Shutdown(const EngineShutdownParams& params, World& world) override;

	void PrintLogMessage(const DebugLogEvent& logEvent);
	
	static void DrawDebugBox(glm::vec3 location, glm::quat rotation, glm::vec3 scale, glm::vec4 color, float duration = 0.0f);
	static void DrawDebugSphere(glm::vec3 location, glm::quat rotation, glm::vec3 scale, glm::vec4 color, float duration = 0.0f);

private:
	void Update(LocalSystemContext& ctx, float deltaTime) override;

	static void UpdateAllWireframes(float deltaTime);

	friend class GraphicsSystem;
	friend class Engine;

private:
	static void SetDebugMaterial(Material* material) { Instance->debugMaterial = material; };

	static void SetDebugBoxMesh(Mesh* mesh) { Instance->debugBoxMesh = mesh; };
	static void SetDebugSphereMesh(Mesh* mesh) { Instance->debugSphereMesh = mesh; };

public:
	REDFORGE_API static std::vector<DebugLogEvent> GetDebugLog() { return Instance->debugLog; }

	static std::unordered_map<DebugEntity, float> GetAllDebugEntities() { return Instance->debugEntities; };

	static const Material* GetDebugMaterial() { return Instance->debugMaterial; };

	REDFORGE_API static Event<const DebugLogEvent&>* GetOnLogMessagePrinted() { return Instance ? &Instance->onLogMessagePrinted : nullptr; }
};