#pragma once

#include "EntityManager.h"
#include "CameraComponent.h"

#include <glm/glm.hpp>

class CameraManager
{
private:
	static inline CameraManager* Instance;

	float targetAspectRatio;

	glm::mat4* viewMatrixOverride = nullptr;
	glm::mat4* projectionMatrixOverride = nullptr;

	Entity mainCameraEntity = -1;

public:
	CameraManager() {};
	~CameraManager() {};

	void Startup();
	void Shutdown();

	static glm::mat4 GetMainViewMatrix();
	static glm::mat4 GetMainProjectionMatrix();
	static glm::mat4 GetViewMatrix(Entity cameraEntity);

	static Entity GetMainCamera() { return Instance->mainCameraEntity; };

	static REDFORGE_API void SetViewMatrixOverride(glm::mat4* viewMatrix) { if(Instance) Instance->viewMatrixOverride = viewMatrix; };
	static REDFORGE_API void SetProjectionMatrixOverride(glm::mat4* projectionMatrix) { if(Instance) Instance->projectionMatrixOverride = projectionMatrix; };

	static REDFORGE_API void SetMainCamera(Entity cameraEntity);

	static void SetTargetAspectRatio(float aspectRatio) { Instance->targetAspectRatio = aspectRatio; };
};