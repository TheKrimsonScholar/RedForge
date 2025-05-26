#pragma once

#include <glm/glm.hpp>

struct PhysicsComponent
{
    bool isStatic;

    float mass = 1;
    float bounciness = 0;
    float staticFriction = 0.5f;
    float dynamicFriction = 0.3f;

    glm::vec3 gravity;

    glm::vec3 velocity;
    glm::vec3 angularVelocity;

    glm::vec3 netForce{ 0, 0, 0 };
    glm::vec3 netTorque{ 0, 0, 0 };

public:
    void AddForce(glm::vec3 force);
    void AddTorque(glm::vec3 axisAngle);
    void ApplyGravity();

    float GetMass() const;
    float GetInverseMass() const;
};