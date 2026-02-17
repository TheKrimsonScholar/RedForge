#include "CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 CameraComponent::GetProjectionMatrix() const
{
    switch(projectionType)
    {
	    case ECameraProjectionType::Perspective:
            return glm::perspective(fov, aspectRatio, nearClipPlaneDistance, farClipPlaneDistance);
        case ECameraProjectionType::Orthographic:
		    return glm::orthoLH(left, right, bottom, top, nearClipPlaneDistance, farClipPlaneDistance);
    }
	
    return glm::mat4(1.0f);
}

glm::mat4 GetViewMatrix(Entity cameraEntity, SystemContext<const TransformComponent> ctx)
{
    assert(ctx.HasComponent<CameraComponent>(cameraEntity) && "Entity is not a camera.");

    const TransformComponent& transform = ctx.GetComponent<const TransformComponent>(cameraEntity);

    return glm::lookAt(transform.location, transform.location + transform.GetForward(), transform.GetUp());
}