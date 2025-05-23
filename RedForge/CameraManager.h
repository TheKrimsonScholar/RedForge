#pragma once

#include "EntityManager.h"
#include "CameraComponent.h"

#include <glm/glm.hpp>

class CameraManager
{
private:
	static inline CameraManager* Instance;

	Entity mainCameraEntity = -1;

public:
	CameraManager() {};
	~CameraManager() {};

	void Startup();
	void Shutdown();

	static glm::mat4 GetViewMatrix(Entity cameraEntity);

	static Entity GetMainCamera() { return Instance->mainCameraEntity; };
	static void SetMainCamera(Entity cameraEntity) { Instance->mainCameraEntity = cameraEntity; };
};