#include "InputSystem.h"

#include <iostream>

#include "GraphicsSystem.h"
#include "CameraManager.h"
#include "TimeManager.h"
#include "LevelManager.h"

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

	for(Entity entity : LevelManager::GetAllEntities())
		if(EntityManager::HasComponent<InputComponent>(entity))
		{
			InputComponent& input = EntityManager::GetComponent<InputComponent>(entity);

			for(auto& mouseCallback : input.mouseDownCallbacks)
			{
				if(InputSystem::IsMouseButtonDown((MouseButtonCode) mouseCallback.first))
					mouseCallback.second(entity);
			}
			
			for(auto& keyCallback : input.keyDownCallbacks)
			{
				if(InputSystem::IsKeyDown((RFKeyCode) keyCallback.first))
					keyCallback.second(entity);
			}
		}

	activeInputLayer->PostUpdate();
}