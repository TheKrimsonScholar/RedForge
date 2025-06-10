#include "ViewportCamera.h"

#include <iostream>

#include "TimeManager.h"
#include "InputSystem.h"
#include "GraphicsSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

ViewportCamera::ViewportCamera() : 
	viewMatrix(1.0f), projectionMatrix(1.0f), location(0, 0, 0), pitchYaw(0, 0)
{
	
}
ViewportCamera::~ViewportCamera()
{

}

void ViewportCamera::Update()
{
	static const float LOOK_SPEED = 1.0f;
	static const float MOVE_SPEED = 5.0f;

	if(InputSystem::IsMouseButtonDown(MouseButtonCode::Left))
	{
		glm::dvec2 mouseDelta = InputSystem::GetMouseDelta();
		pitchYaw.x -= mouseDelta.y * TimeManager::GetDeltaTime() * LOOK_SPEED;
		pitchYaw.y += mouseDelta.x * TimeManager::GetDeltaTime() * LOOK_SPEED;
	}

	if(InputSystem::IsKeyDown(KeyCode::A))
		location -= GetRight() * TimeManager::GetDeltaTime() * MOVE_SPEED;
	if(InputSystem::IsKeyDown(KeyCode::D))
		location += GetRight() * TimeManager::GetDeltaTime() * MOVE_SPEED;
	if(InputSystem::IsKeyDown(KeyCode::S))
		location -= GetForward() * TimeManager::GetDeltaTime() * MOVE_SPEED;
	if(InputSystem::IsKeyDown(KeyCode::W))
		location += GetForward() * TimeManager::GetDeltaTime() * MOVE_SPEED;
	if(InputSystem::IsKeyDown(KeyCode::LSHIFT))
		location -= glm::vec3(0, 1, 0) * TimeManager::GetDeltaTime() * MOVE_SPEED;
	if(InputSystem::IsKeyDown(KeyCode::SPACE))
		location += glm::vec3(0, 1, 0) * TimeManager::GetDeltaTime() * MOVE_SPEED;

	viewMatrix = glm::lookAt(location, location + GetForward(), -GetUp()); // Invert up vector to counteract Vulkan-OpenGL differences
	projectionMatrix = glm::perspective(glm::radians(fov), GraphicsSystem::GetAspectRatio(), nearClipPlaneDistance, farClipPlaneDistance);
}

glm::vec3 ViewportCamera::GetRight()
{
	return glm::quat(glm::vec3(pitchYaw, 0)) * glm::vec3(-1, 0, 0);
}
glm::vec3 ViewportCamera::GetUp()
{
	return glm::quat(glm::vec3(pitchYaw, 0)) * glm::vec3(0, 1, 0);
}
glm::vec3 ViewportCamera::GetForward()
{
	return glm::quat(glm::vec3(pitchYaw, 0)) * glm::vec3(0, 0, -1);
}