#pragma once

#include "Exports.h"

#include <glm/glm.hpp>

enum class RFKeyCode : uint32_t
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	LSHIFT, RSHIFT, SPACE, ESCAPE, LCTRL, RCTRL, LALT, RALT,
	MAX
};
enum class MouseButtonCode
{
	Left,
	Right,
	Middle,
	MAX
};

class InputLayer
{
protected:
	bool keysDown[(size_t) RFKeyCode::MAX] = {};
	bool keysPressedThisFrame[(size_t) RFKeyCode::MAX] = {};
	bool keysReleasedThisFrame[(size_t) RFKeyCode::MAX] = {};

	bool mouseButtonsDown[(size_t) MouseButtonCode::MAX] = {};
	bool mouseButtonsPressedThisFrame[(size_t) MouseButtonCode::MAX] = {};
	bool mouseButtonsReleasedThisFrame[(size_t) MouseButtonCode::MAX] = {};

	glm::dvec2 mousePosition = { 0, 0 };
	glm::dvec2 mouseDelta = { 0, 0 };

public:
	REDFORGE_API virtual ~InputLayer();

	REDFORGE_API virtual void PreUpdate();
	REDFORGE_API virtual void PostUpdate();

	REDFORGE_API glm::dvec2 GetMousePosition() { return mousePosition; };
	REDFORGE_API glm::dvec2 GetMouseDelta() { return mouseDelta; };

	REDFORGE_API bool IsKeyDown(RFKeyCode key) { return keysDown[(size_t) key]; };
	REDFORGE_API bool WasKeyPressedThisFrame(RFKeyCode key) { return keysPressedThisFrame[(size_t) key]; };
	REDFORGE_API bool WasKeyReleasedThisFrame(RFKeyCode key) { return keysReleasedThisFrame[(size_t) key]; };
	REDFORGE_API bool IsMouseButtonDown(MouseButtonCode mouseButton) { return mouseButtonsDown[(size_t) mouseButton]; };
	REDFORGE_API bool WasMouseButtonPressedThisFrame(MouseButtonCode mouseButton) { return mouseButtonsPressedThisFrame[(size_t) mouseButton]; };
	REDFORGE_API bool WasMouseButtonReleasedThisFrame(MouseButtonCode mouseButton) { return mouseButtonsReleasedThisFrame[(size_t) mouseButton]; };
};