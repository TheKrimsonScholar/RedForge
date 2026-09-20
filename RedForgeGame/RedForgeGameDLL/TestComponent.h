#pragma once

#include "RedForgeGame.h"

#include "ComponentRegistration.h"

REDFORGEGAME_API struct TestComponent
{
	SERIALIZED_COMPONENT_VARS()
	REPLICATED_COMPONENT_VARS()

	float k;
};

REGISTER_COMPONENT_TYPE_BEGIN(TestComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VAR(float, k)
COMPONENT_VARS_END
REGISTER_COMPONENT_TYPE_END(TestComponent)

//struct TestComponent
//{
//	float k;
//
//	void Initialize();
//	void Update();
//};
//
//class TestComponent_System
//{
//	void Initialize()
//	{
//		EntityManager::ForEachComponentOfType<TestComponent>(
//			[](const Entity& entity, TestComponent& component)
//			{
//				component.Initialize();
//			});
//	}
//	void Update()
//	{
//		EntityManager::ForEachComponentOfType<TestComponent>(
//			[](const Entity& entity, TestComponent& component)
//			{
//				component.Update();
//			});
//	}
//};