#include "InputSystem.h"

#include <iostream>

#include "GraphicsSystem.h"
#include "CameraManager.h"
#include "TimeManager.h"
#include "LevelManager.h"

#include "TransformComponent.h"
#include "InputComponent.h"

#include "DebugMacros.h"

#include "include/imgui/imgui.h"

static std::unordered_map<MouseButtonCode, ImGuiMouseButton> IMGUI_INPUT_MOUSE_BUTTON_MAP =
{
    { MouseButtonCode::Left, ImGuiMouseButton_Left },
    { MouseButtonCode::Middle, ImGuiMouseButton_Middle },
    { MouseButtonCode::Right, ImGuiMouseButton_Right }
};
static std::unordered_map<RFKeyCode, ImGuiKey> IMGUI_INPUT_KEY_MAP =
{
    { RFKeyCode::A, ImGuiKey_A },
    { RFKeyCode::B, ImGuiKey_B },
    { RFKeyCode::C, ImGuiKey_C },
    { RFKeyCode::D, ImGuiKey_D },
    { RFKeyCode::E, ImGuiKey_E },
    { RFKeyCode::F, ImGuiKey_F },
    { RFKeyCode::G, ImGuiKey_G },
    { RFKeyCode::H, ImGuiKey_H },
    { RFKeyCode::I, ImGuiKey_I },
    { RFKeyCode::J, ImGuiKey_J },
    { RFKeyCode::K, ImGuiKey_K },
    { RFKeyCode::L, ImGuiKey_L },
    { RFKeyCode::M, ImGuiKey_M },
    { RFKeyCode::N, ImGuiKey_N },
    { RFKeyCode::O, ImGuiKey_O },
    { RFKeyCode::P, ImGuiKey_P },
    { RFKeyCode::Q, ImGuiKey_Q },
    { RFKeyCode::R, ImGuiKey_R },
    { RFKeyCode::S, ImGuiKey_S },
    { RFKeyCode::T, ImGuiKey_T },
    { RFKeyCode::U, ImGuiKey_U },
    { RFKeyCode::V, ImGuiKey_V },
    { RFKeyCode::W, ImGuiKey_W },
    { RFKeyCode::X, ImGuiKey_X },
    { RFKeyCode::Y, ImGuiKey_Y },
    { RFKeyCode::Z, ImGuiKey_Z },

    { RFKeyCode::LSHIFT, ImGuiKey_LeftShift },
    { RFKeyCode::RSHIFT, ImGuiKey_RightShift },
    { RFKeyCode::SPACE, ImGuiKey_Space },
    { RFKeyCode::ESCAPE, ImGuiKey_Escape },
    { RFKeyCode::LCTRL, ImGuiKey_LeftCtrl },
    { RFKeyCode::RCTRL, ImGuiKey_RightCtrl },
    { RFKeyCode::LALT, ImGuiKey_LeftAlt },
    { RFKeyCode::RALT, ImGuiKey_RightAlt }
};

void InputSystem::Startup()
{
	Instance = this;
}
void InputSystem::Shutdown()
{

}

void InputSystem::Update()
{
	if(!activeInputLayer)
		return;

	activeInputLayer->PreUpdate();

#ifdef _DEBUG
	/* Pass input to ImGui for debug builds */

	ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(GetMousePosition().x, GetMousePosition().y);
    for(uint32_t i = 0; i < (uint32_t) MouseButtonCode::MAX; i++)
	    io.AddMouseButtonEvent(IMGUI_INPUT_MOUSE_BUTTON_MAP[(MouseButtonCode) i], IsMouseButtonDown((MouseButtonCode) i));
    for(uint32_t i = 0; i < (uint32_t) RFKeyCode::MAX; i++)
	    io.AddKeyEvent(IMGUI_INPUT_KEY_MAP[(RFKeyCode) i], IsKeyDown((RFKeyCode) i));
#endif

	/* Update all active input components */

    EntityManager::ForEachComponentOfType<InputComponent>(
        [this](const Entity& entity, InputComponent& input)
		{
			for(auto& mouseCallback : input.mouseDownCallbacks)
			{
				if(InputSystem::IsMouseButtonDown((MouseButtonCode) mouseCallback.first))
					mouseCallback.second(entity);
			}
			
			for(auto& keyCallback : input.keyDownCallbacks)
			{
				if(InputSystem::IsKeyDown((RFKeyCode) keyCallback.first))
					keyCallback.second(entity);
			}
		});

	activeInputLayer->PostUpdate();
}