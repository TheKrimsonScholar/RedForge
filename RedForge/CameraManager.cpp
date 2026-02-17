#include "CameraManager.h"

#include "TransformComponent.h"

#include "GraphicsSystem.h"

void CameraManager::Startup(const EngineStartupParams& params, World& world)
{
	Instance = this;
}
void CameraManager::PostStartup(const EngineStartupParams& params, World& world)
{

}
void CameraManager::Shutdown(const EngineShutdownParams& params, World& world)
{

}

void CameraManager::Update(SystemContext<const Cameras, const Window, CameraComponent>& ctx, float deltaTime)
{
	const Cameras& cameras = ctx.GetResource<const Cameras>();
	const Window& window = ctx.GetResource<const Window>();
	
	if(cameras.mainCameraEntity.IsValid())
	{
		CameraComponent& mainCamera = ctx.GetComponent<CameraComponent>(cameras.mainCameraEntity);
		// Update the relevant camera properties controlled by the viewport
		mainCamera.aspectRatio = window.GetAspectRatio();
	}
}