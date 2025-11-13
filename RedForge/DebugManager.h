#pragma once

#include <unordered_map>

#include "Mesh.h"
#include "Event.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Exports.h"

enum class LogType
{
	Cout,
	Engine,
	Editor,
	Game
};
struct LogMessage
{
	double timestamp;
	LogType logType;
	std::string text;

	REDFORGE_API std::string ToString() const;
};

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

class DebugManager
{
private:
	static inline DebugManager* Instance;

	Event<const LogMessage&> onLogMessagePrinted;

	std::streambuf* coutBuffer = nullptr;
	std::stringstream coutStream;
	// The ending stream position of the last complete line read from cout.
	size_t lastCoutReadPosition = 0;

	Material* debugMaterial;

	Mesh* debugBoxMesh;
	Mesh* debugSphereMesh;

	std::vector<LogMessage> debugLog;
	std::unordered_map<DebugEntity, float> debugEntities;

public:
	DebugManager() {};
	~DebugManager() {};

	void Startup();
	void Shutdown();

	static void PrintLogMessage(LogType logType, const std::string& text);
	
	static void DrawDebugBox(glm::vec3 location, glm::quat rotation, glm::vec3 scale, glm::vec4 color, float duration = 0.0f);
	static void DrawDebugSphere(glm::vec3 location, glm::quat rotation, glm::vec3 scale, glm::vec4 color, float duration = 0.0f);

private:
	void Update();

	static void UpdateAllWireframes();

	friend class GraphicsSystem;
	friend class Engine;

private:
	static void SetDebugMaterial(Material* material) { Instance->debugMaterial = material; };

	static void SetDebugBoxMesh(Mesh* mesh) { Instance->debugBoxMesh = mesh; };
	static void SetDebugSphereMesh(Mesh* mesh) { Instance->debugSphereMesh = mesh; };

public:
	REDFORGE_API static std::vector<LogMessage> GetDebugLog() { return Instance->debugLog; }

	static std::unordered_map<DebugEntity, float> GetAllDebugEntities() { return Instance->debugEntities; };

	static Material* GetDebugMaterial() { return Instance->debugMaterial; };

	REDFORGE_API static Event<const LogMessage&>* GetOnLogMessagePrinted() { return Instance ? &Instance->onLogMessagePrinted : nullptr; }
};