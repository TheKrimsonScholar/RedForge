#include "InputLayer.h"

InputLayer::~InputLayer()
{
	
}

void InputLayer::PreUpdate()
{
	static glm::dvec2 previousMousePosition = mousePosition;

	mouseDelta = mousePosition - previousMousePosition;
	previousMousePosition = mousePosition;
}
void InputLayer::PostUpdate()
{
	// Update single-frame states
	for(uint32_t i = 0; i < (uint32_t) KeyCode::MAX; i++)
	{
		if(keysPressedThisFrame[i])
			keysPressedThisFrame[i] = false;
		if(keysReleasedThisFrame[i])
			keysReleasedThisFrame[i] = false;
	}
}