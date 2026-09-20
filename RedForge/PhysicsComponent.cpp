#include "PhysicsComponent.h"

REGISTER_COMPONENT_TYPE_BEGIN(PhysicsComponent, IComponent)
COMPONENT_REQUIRES(TransformComponent)
COMPONENT_VARS_BEGIN
COMPONENT_VAR(float, mass)
COMPONENT_VAR(float, bounciness)
COMPONENT_VAR(float, staticFriction)
COMPONENT_VAR(float, dynamicFriction)
COMPONENT_VAR(glm::vec3, gravity)
COMPONENT_VAR(glm::vec3, velocity)
COMPONENT_VAR(glm::vec3, angularVelocity)
COMPONENT_VARS_END
REGISTER_COMPONENT_TYPE_END(PhysicsComponent)

void PhysicsComponent::AddForce(glm::vec3 force)
{
	if(isStatic)
		return;

	netForce += force;
}
void PhysicsComponent::AddTorque(glm::vec3 axisAngle)
{
	if(isStatic)
		return;

	netTorque += axisAngle;
}
void PhysicsComponent::ApplyGravity()
{
	AddForce(gravity);
}

float PhysicsComponent::GetMass() const
{
	// Static bodies should be treated as having infinite mass
	return isStatic ? FLT_MAX : mass;
}
float PhysicsComponent::GetInverseMass() const
{
	return isStatic ? 0 : 1.0f / mass;
}