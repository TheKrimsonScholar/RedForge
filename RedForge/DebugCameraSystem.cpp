#include "DebugCameraSystem.h"

void DebugCameraSystem::Startup(const EngineStartupParams& params, World& world)
{
	
}
void DebugCameraSystem::PostStartup(const EngineStartupParams& params, World& world)
{
	DebugCameraState& cameraState = world.GetResource<DebugCameraState>();
	Cameras& cameras = world.GetResource<Cameras>();
	const Window& window = world.GetResource<const Window>();

	if(cameraState.isActive)
	{
		cameraState.viewMatrix = glm::lookAt(cameraState.location, cameraState.location + cameraState.GetForward(), cameraState.GetUp());
		cameraState.projectionMatrix = glm::perspective(glm::radians(cameraState.fov), window.GetAspectRatio(), cameraState.nearClipPlaneDistance, cameraState.farClipPlaneDistance);

		cameras.viewMatrixOverride = &cameraState.viewMatrix;
		cameras.projectionMatrixOverride = &cameraState.projectionMatrix;
	}
}
void DebugCameraSystem::Shutdown(const EngineShutdownParams& params, World& world)
{

}

void DebugCameraSystem::Update(SystemContext<DebugCameraState, Cameras, const InputState, const Window>& ctx, float deltaTime)
{
	DebugCameraState& cameraState = ctx.GetResource<DebugCameraState>();
	Cameras& cameras = ctx.GetResource<Cameras>();
	const InputState& inputState = ctx.GetResource<const InputState>();
	const Window& window = ctx.GetResource<const Window>();

	if(cameraState.isActive)
	{
		if(inputState.IsMouseButtonDown(MouseButtonCode::Left))
		{
			glm::dvec2 mouseDelta = inputState.mouseDelta;
			cameraState.pitchYaw.x -= mouseDelta.y * deltaTime * cameraState.lookSpeed;
			cameraState.pitchYaw.y -= mouseDelta.x * deltaTime * cameraState.lookSpeed;
		}

		if(inputState.IsKeyDown(RFKeyCode::A))
			cameraState.location -= cameraState.GetRight() * deltaTime * cameraState.moveSpeed;
		if(inputState.IsKeyDown(RFKeyCode::D))
			cameraState.location += cameraState.GetRight() * deltaTime * cameraState.moveSpeed;
		if(inputState.IsKeyDown(RFKeyCode::S))
			cameraState.location -= cameraState.GetForward() * deltaTime * cameraState.moveSpeed;
		if(inputState.IsKeyDown(RFKeyCode::W))
			cameraState.location += cameraState.GetForward() * deltaTime * cameraState.moveSpeed;
		if(inputState.IsKeyDown(RFKeyCode::LSHIFT))
			cameraState.location -= glm::vec3(0, 1, 0) * deltaTime * cameraState.moveSpeed;
		if(inputState.IsKeyDown(RFKeyCode::SPACE))
			cameraState.location += glm::vec3(0, 1, 0) * deltaTime * cameraState.moveSpeed;

		cameraState.viewMatrix = glm::lookAt(cameraState.location, cameraState.location + cameraState.GetForward(), cameraState.GetUp());
		cameraState.projectionMatrix = glm::perspective(glm::radians(cameraState.fov), window.GetAspectRatio(), cameraState.nearClipPlaneDistance, cameraState.farClipPlaneDistance);

		cameras.viewMatrixOverride = &cameraState.viewMatrix;
		cameras.projectionMatrixOverride = &cameraState.projectionMatrix;
	}
	// If debug camera is not active, clear overrides if they point to the debug camera
	else
	{
		if(cameras.viewMatrixOverride == &cameraState.viewMatrix)
			cameras.viewMatrixOverride = nullptr;
		if(cameras.projectionMatrixOverride == &cameraState.projectionMatrix)
			cameras.projectionMatrixOverride = nullptr;
	}
}