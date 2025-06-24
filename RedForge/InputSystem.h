#pragma once

#include "InputLayer.h"

#include "Exports.h"

#include <glm/glm.hpp>

class InputSystem
{
private:
	static const uint32_t NUM_BUTTONS = 512;

private:
	static inline InputSystem* Instance;

	InputLayer* activeInputLayer = nullptr;

	bool buttonsDown[NUM_BUTTONS] = {};
	bool buttonsPressedThisFrame[NUM_BUTTONS] = {};

	glm::dvec2 mousePosition = { 0, 0 };
	glm::dvec2 mouseDelta = { 0, 0 };

public:
	InputSystem() {};
	~InputSystem() {};

	void Startup();
	void Shutdown();
	
	REDFORGE_API void Update();

	REDFORGE_API static bool IsKeyDown(RFKeyCode key) { return Instance->activeInputLayer ? Instance->activeInputLayer->IsKeyDown(key) : false; };
	REDFORGE_API static bool WasButtonFirstPressedThisFrame(uint32_t buttonIndex) { return Instance->buttonsPressedThisFrame[buttonIndex]; };

	REDFORGE_API static glm::dvec2 GetMousePosition() { return Instance->activeInputLayer ? Instance->activeInputLayer->GetMousePosition() : glm::dvec2(0, 0); };
	REDFORGE_API static glm::dvec2 GetMouseDelta() { return Instance->activeInputLayer ? Instance->activeInputLayer->GetMouseDelta() : glm::dvec2(0, 0); };
	
	REDFORGE_API static bool IsMouseButtonDown(MouseButtonCode mouseButton) { return Instance->activeInputLayer ? Instance->activeInputLayer->IsMouseButtonDown(mouseButton) : false; };

	REDFORGE_API static void SetActiveInputLayer(InputLayer* inputLayer) { Instance->activeInputLayer = inputLayer; };
};