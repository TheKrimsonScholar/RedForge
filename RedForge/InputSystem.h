#pragma once

#include <glm/glm.hpp>

class InputSystem
{
private:
	static const uint32_t NUM_BUTTONS = 512;

private:
	static inline InputSystem* Instance;

	bool buttonsDown[NUM_BUTTONS] = {};
	bool buttonsPressedThisFrame[NUM_BUTTONS] = {};

	glm::dvec2 mousePosition = { 0, 0 };
	glm::dvec2 mouseDelta = { 0, 0 };

public:
	InputSystem() {};
	~InputSystem() {};

	void Startup();
	void Shutdown();
	
	void Update();

	static bool WasButtonFirstPressedThisFrame(uint32_t buttonIndex) { return Instance->buttonsPressedThisFrame[buttonIndex]; };

	static glm::dvec2 GetMousePosition() { return Instance->mousePosition; };
	static glm::dvec2 GetMouseDelta() { return Instance->mouseDelta; };
};