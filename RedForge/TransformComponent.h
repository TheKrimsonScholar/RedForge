#pragma once

#include "ComponentRegistrationMacros.h"

#include "Exports.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

REDFORGE_API struct TransformComponent
{
	glm::vec3 location = { 0, 0, 0 };
	glm::quat rotation = { 0, 0, 0, 1 };
	glm::vec3 scale = { 1, 1, 1 };

public:
	void MoveRelative(glm::vec3 vector);

	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetForward() const;

	glm::mat4 GetMatrix() const;
	glm::mat4 GetRotationMatrix() const;

	glm::vec3 LocalToWorld_Point(const glm::vec3& vector, bool includeScale = false) const;
	glm::vec3 LocalToWorld_Direction(const glm::vec3& vector, bool includeScale = false) const;
};

//struct RegisterComponent_TransformComponent
//{
//	RegisterComponent_TransformComponent()
//	{
//		GetMembersMap().emplace(typeid(TransformComponent), [](void* rawComponent) -> std::unordered_map<void*, ComponentVariableInfo>
//			{
//				TransformComponent* component = static_cast<TransformComponent*>(rawComponent);
//
//				return
//				{
//					{ &component->location, { typeid(glm::vec3), "location" }},
//					{ &component->scale, { typeid(glm::vec3), "scale" }}
//				};
//			});
//	}
//};
//
//static RegisterComponent_TransformComponent k;

REGISTER_COMPONENT_BEGIN(TransformComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VAR(glm::vec3, location)
COMPONENT_VAR(glm::quat, rotation)
COMPONENT_VAR(glm::vec3, scale)
COMPONENT_VARS_END
REGISTER_COMPONENT_END(TransformComponent)

//COMPONENT_BEGIN(TransformComponent)
//COMPONENT_VAR(glm::vec3, location)
//COMPONENT_VAR(glm::quat, rotation)
//COMPONENT_VAR(glm::vec3, scale)
//COMPONENT_END()