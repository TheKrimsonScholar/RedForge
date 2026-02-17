#pragma once

#include "ResourceRegistrationMacros.h"

#include "EngineParams.h"

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

class InputLayer;

struct InputState : public IResource
{
private:
	InputLayer* activeInputLayer = nullptr;

public:
	bool keysDown[(size_t) RFKeyCode::MAX] = {};
	bool keysPressedThisFrame[(size_t) RFKeyCode::MAX] = {};
	bool keysReleasedThisFrame[(size_t) RFKeyCode::MAX] = {};

	bool mouseButtonsDown[(size_t) MouseButtonCode::MAX] = {};
	bool mouseButtonsPressedThisFrame[(size_t) MouseButtonCode::MAX] = {};
	bool mouseButtonsReleasedThisFrame[(size_t) MouseButtonCode::MAX] = {};

	glm::dvec2 mousePosition = { 0, 0 };
	glm::dvec2 mouseDelta = { 0, 0 };

private:
	REDFORGE_API void Startup(const EngineStartupParams& params, World& world) override;
	REDFORGE_API void Shutdown(const EngineShutdownParams& params, World& world) override;

public:
	REDFORGE_API bool IsKeyDown(RFKeyCode key) const { return keysDown[(size_t) key]; };
	REDFORGE_API bool WasKeyPressedThisFrame(RFKeyCode key) const { return keysPressedThisFrame[(size_t) key]; };
	REDFORGE_API bool WasKeyReleasedThisFrame(RFKeyCode key) const { return keysReleasedThisFrame[(size_t) key]; };
	REDFORGE_API bool IsMouseButtonDown(MouseButtonCode mouseButton) const { return mouseButtonsDown[(size_t) mouseButton]; };
	REDFORGE_API bool WasMouseButtonPressedThisFrame(MouseButtonCode mouseButton) const { return mouseButtonsPressedThisFrame[(size_t) mouseButton]; };
	REDFORGE_API bool WasMouseButtonReleasedThisFrame(MouseButtonCode mouseButton) const { return mouseButtonsReleasedThisFrame[(size_t) mouseButton]; };

	InputLayer* GetActiveInputLayer() { return activeInputLayer; }
	void SetActiveInputLayer(const EngineStartupParams& params, InputLayer* inputLayer) { activeInputLayer = inputLayer; }
};
REGISTER_RESOURCE(InputState)