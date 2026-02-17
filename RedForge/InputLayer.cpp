#include "InputLayer.h"

InputLayer::~InputLayer()
{
	
}

void InputLayer::PreUpdate(InputState& inputState)
{
	static glm::dvec2 previousMousePosition = inputState.mousePosition;

	inputState.mouseDelta = inputState.mousePosition - previousMousePosition;
	previousMousePosition = inputState.mousePosition;
}
void InputLayer::PostUpdate(InputState& inputState)
{
	// Update single-frame states
	for(uint32_t i = 0; i < (uint32_t) RFKeyCode::MAX; i++)
	{
		if(inputState.keysPressedThisFrame[i])
			inputState.keysPressedThisFrame[i] = false;
		if(inputState.keysReleasedThisFrame[i])
			inputState.keysReleasedThisFrame[i] = false;
	}
}