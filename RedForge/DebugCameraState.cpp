#include "DebugCameraState.h"

#include <glm/gtc/quaternion.hpp>

void DebugCameraState::Startup(const EngineStartupParams& params, World& world)
{

}
void DebugCameraState::Shutdown(const EngineShutdownParams& params, World& world)
{

}

glm::vec3 DebugCameraState::GetRight() const
{
	return glm::quat(glm::vec3(pitchYaw, 0)) * glm::vec3(1, 0, 0);
}
glm::vec3 DebugCameraState::GetUp() const
{
	return glm::quat(glm::vec3(pitchYaw, 0)) * glm::vec3(0, 1, 0);
}
glm::vec3 DebugCameraState::GetForward() const
{
	return glm::quat(glm::vec3(pitchYaw, 0)) * glm::vec3(0, 0, -1);
}