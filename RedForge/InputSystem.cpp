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
	if(!activeInputLayer)
		return;

	activeInputLayer->PreUpdate();

	for(Entity e = 0; e < EntityManager::GetLastEntity(); e++)
		if(EntityManager::HasComponent<InputComponent>(e))
		{
			InputComponent& input = EntityManager::GetComponent<InputComponent>(e);

			for(auto& mouseCallback : input.mouseDownCallbacks)
			{
				if(InputSystem::IsMouseButtonDown((MouseButtonCode) mouseCallback.first))
					mouseCallback.second(e);
			}
			
			for(auto& keyCallback : input.keyDownCallbacks)
			{
				if(InputSystem::IsKeyDown((RFKeyCode) keyCallback.first))
					keyCallback.second(e);
			}
		}

	activeInputLayer->PostUpdate();
}