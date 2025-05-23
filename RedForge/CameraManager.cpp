#include "CameraManager.h"

#include "TransformComponent.h"

void CameraManager::Startup()
{
	Instance = this;
}
void CameraManager::Shutdown()
{

}

glm::mat4 CameraManager::GetViewMatrix(Entity cameraEntity)
{
    assert(EntityManager::HasComponent<CameraComponent>(cameraEntity) && "Entity is not a camera.");

	TransformComponent& transform = EntityManager::GetComponent<TransformComponent>(cameraEntity);
	CameraComponent& camera = EntityManager::GetComponent<CameraComponent>(cameraEntity);

    return glm::lookAt(transform.location, transform.location + transform.GetForward(), transform.GetUp());
}