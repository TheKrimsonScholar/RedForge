#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class ViewportCamera
{
private:
	float fov = 60.0f;
	float nearClipPlaneDistance = 0.001f;
	float farClipPlaneDistance = 10000.0f;

public:
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

private:
	glm::vec3 location;

	glm::vec2 pitchYaw;

public:
	ViewportCamera();
	~ViewportCamera();

	void Update(GLFWwindow* window);

	glm::vec3 GetRight();
	glm::vec3 GetUp();
	glm::vec3 GetForward();
};