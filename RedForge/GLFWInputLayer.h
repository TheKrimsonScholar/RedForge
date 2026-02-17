#pragma once

#include "InputLayer.h"

#include <GLFW/glfw3.h>

class GLFWInputLayer : public InputLayer
{
private:
	GLFWwindow* window;

public:
	REDFORGE_API GLFWInputLayer();
	GLFWInputLayer(GLFWwindow* window);
	REDFORGE_API ~GLFWInputLayer() override;

	void Startup(const EngineStartupParams& params, InputState& inputState) override;
	void Shutdown(const EngineShutdownParams& params, InputState& inputState) override;

	void PreUpdate(InputState& inputState) override;
	void PostUpdate(InputState& inputState) override;
};