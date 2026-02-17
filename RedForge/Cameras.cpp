#include "Cameras.h"

void Cameras::Startup(const EngineStartupParams& params, World& world)
{

}
void Cameras::Shutdown(const EngineShutdownParams& params, World& world)
{

}

glm::mat4 Cameras::GetMainViewMatrix(SystemContext<const TransformComponent> ctx) const
{
	return viewMatrixOverride ?
		*viewMatrixOverride : GetViewMatrix(mainCameraEntity, ctx);
}
glm::mat4 Cameras::GetMainProjectionMatrix(SystemContext<const CameraComponent> ctx) const
{
	return projectionMatrixOverride ?
		*projectionMatrixOverride : ctx.GetComponent<const CameraComponent>(mainCameraEntity).GetProjectionMatrix();
}