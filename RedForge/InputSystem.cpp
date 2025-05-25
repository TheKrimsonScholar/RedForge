#include "InputSystem.h"

#include <iostream>

#include "GraphicsSystem.h"
#include "CameraManager.h"
#include "TimeManager.h"

#include "TransformComponent.h"
#include "InputComponent.h"

void InputSystem::Startup()
{
	Instance = this;
}
void InputSystem::Shutdown()
{

}

void InputSystem::Update()
{
	static glm::dvec2 previousMousePosition = mousePosition;

	glfwGetCursorPos(GraphicsSystem::GetWindow(), &mousePosition.x, &mousePosition.y);
	// Ignore mouse delta on first frame
	if(TimeManager::GetCurrentFrame() > 0)
		mouseDelta = mousePosition - previousMousePosition;
	previousMousePosition = mousePosition;

	if(glfwGetKey(GraphicsSystem::GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(GraphicsSystem::GetWindow(), true);

	for(Entity e = 0; e < EntityManager::GetLastEntity(); e++)
		if(EntityManager::HasComponent<InputComponent>(e))
		{
			InputComponent& input = EntityManager::GetComponent<InputComponent>(e);

			for(auto& mouseCallback : input.mouseDownCallbacks)
			{
				// This button was just pressed this frame if it was not pressed last frame and is pressed this frame
				buttonsPressedThisFrame[mouseCallback.first] = 
					!buttonsDown[mouseCallback.first] && glfwGetMouseButton(GraphicsSystem::GetWindow(), mouseCallback.first) == GLFW_PRESS;
				// Update stored button state
				buttonsDown[mouseCallback.first] = glfwGetMouseButton(GraphicsSystem::GetWindow(), mouseCallback.first) == GLFW_PRESS;

				if(glfwGetMouseButton(GraphicsSystem::GetWindow(), mouseCallback.first) == GLFW_PRESS)
					mouseCallback.second(e);
			}
			
			for(auto& keyCallback : input.keyDownCallbacks)
			{
				// This button was just pressed this frame if it was not pressed last frame and is pressed this frame
				buttonsPressedThisFrame[keyCallback.first] =
					!buttonsDown[keyCallback.first] && glfwGetMouseButton(GraphicsSystem::GetWindow(), keyCallback.first) == GLFW_PRESS;
				// Update stored button state
				buttonsDown[keyCallback.first] = glfwGetMouseButton(GraphicsSystem::GetWindow(), keyCallback.first) == GLFW_PRESS;

				if(glfwGetKey(GraphicsSystem::GetWindow(), keyCallback.first) == GLFW_PRESS)
					keyCallback.second(e);
			}
		}
}