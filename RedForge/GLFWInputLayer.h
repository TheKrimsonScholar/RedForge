#pragma once

#include "InputLayer.h"

#include <glfw/glfw3.h>

class GLFWInputLayer : public InputLayer
{
private:
	GLFWwindow* window;

public:
	REDFORGE_API GLFWInputLayer();
	GLFWInputLayer(GLFWwindow* window);
	REDFORGE_API ~GLFWInputLayer() override;

	void PreUpdate() override;
	void PostUpdate() override;
};