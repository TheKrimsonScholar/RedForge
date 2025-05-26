#include "PhysicsComponent.h"

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