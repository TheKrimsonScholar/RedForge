#include "CameraManager.h"

#include "TransformComponent.h"

#include "GraphicsSystem.h"

void CameraManager::Startup()
{
	Instance = this;

	SetTargetAspectRatio(GraphicsSystem::GetAspectRatio());
}
void CameraManager::Shutdown()
{

}

glm::mat4 CameraManager::GetMainViewMatrix()
{
	return Instance->viewMatrixOverride ? 
		*Instance->viewMatrixOverride : GetViewMatrix(CameraManager::GetMainCamera());
}
glm::mat4 CameraManager::GetMainProjectionMatrix()
{
	return Instance->projectionMatrixOverride ? 
		*Instance->projectionMatrixOverride : EntityManager::GetComponent<CameraComponent>(GetMainCamera()).GetProjectionMatrix();
}
glm::mat4 CameraManager::GetViewMatrix(Entity cameraEntity)
{
    assert(EntityManager::HasComponent<CameraComponent>(cameraEntity) && "Entity is not a camera.");

	TransformComponent& transform = EntityManager::GetComponent<TransformComponent>(cameraEntity);
	CameraComponent& camera = EntityManager::GetComponent<CameraComponent>(cameraEntity);

    return glm::lookAt(transform.location, transform.location + transform.GetForward(), transform.GetUp());
}

void CameraManager::SetMainCamera(Entity cameraEntity)
{
	CameraComponent& newMainCamera = EntityManager::GetComponent<CameraComponent>(cameraEntity);

	Instance->mainCameraEntity = cameraEntity;

	// Update the relevant camera properties controlled by the viewport/camera manager
	newMainCamera.aspectRatio = Instance->targetAspectRatio;
}