#pragma once

#include <glm/glm.hpp>

class InputSystem
{
private:
	static inline InputSystem* Instance;

	glm::dvec2 mousePosition = { 0, 0 };
	glm::dvec2 mouseDelta = { 0, 0 };

public:
	InputSystem() {};
	~InputSystem() {};

	void Startup();
	void Shutdown();
	
	void Update();

	static glm::dvec2 GetMousePosition() { return Instance->mousePosition; };
	static glm::dvec2 GetMouseDelta() { return Instance->mouseDelta; };
};