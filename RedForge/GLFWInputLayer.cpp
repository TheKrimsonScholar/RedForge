#include "GLFWInputLayer.h"

#include <unordered_map>

#include "TimeManager.h"

static std::unordered_map<MouseButtonCode, int> GLFW_INPUT_MOUSE_BUTTON_MAP =
{
    { MouseButtonCode::Left, GLFW_MOUSE_BUTTON_LEFT },
    { MouseButtonCode::Middle, GLFW_MOUSE_BUTTON_MIDDLE },
    { MouseButtonCode::Right, GLFW_MOUSE_BUTTON_RIGHT }
};
static std::unordered_map<RFKeyCode, int> GLFW_INPUT_KEY_MAP =
{
    { RFKeyCode::A, GLFW_KEY_A },
    { RFKeyCode::B, GLFW_KEY_B },
    { RFKeyCode::C, GLFW_KEY_C },
    { RFKeyCode::D, GLFW_KEY_D },
    { RFKeyCode::E, GLFW_KEY_E },
    { RFKeyCode::F, GLFW_KEY_F },
    { RFKeyCode::G, GLFW_KEY_G },
    { RFKeyCode::H, GLFW_KEY_H },
    { RFKeyCode::I, GLFW_KEY_I },
    { RFKeyCode::J, GLFW_KEY_J },
    { RFKeyCode::K, GLFW_KEY_K },
    { RFKeyCode::L, GLFW_KEY_L },
    { RFKeyCode::M, GLFW_KEY_M },
    { RFKeyCode::N, GLFW_KEY_N },
    { RFKeyCode::O, GLFW_KEY_O },
    { RFKeyCode::P, GLFW_KEY_P },
    { RFKeyCode::Q, GLFW_KEY_Q },
    { RFKeyCode::R, GLFW_KEY_R },
    { RFKeyCode::S, GLFW_KEY_S },
    { RFKeyCode::T, GLFW_KEY_T },
    { RFKeyCode::U, GLFW_KEY_U },
    { RFKeyCode::V, GLFW_KEY_V },
    { RFKeyCode::W, GLFW_KEY_W },
    { RFKeyCode::X, GLFW_KEY_X },
    { RFKeyCode::Y, GLFW_KEY_Y },
    { RFKeyCode::Z, GLFW_KEY_Z },

    { RFKeyCode::LSHIFT, GLFW_KEY_LEFT_SHIFT },
    { RFKeyCode::RSHIFT, GLFW_KEY_RIGHT_SHIFT },
    { RFKeyCode::SPACE, GLFW_KEY_SPACE },
    { RFKeyCode::ESCAPE, GLFW_KEY_ESCAPE },
    { RFKeyCode::LCTRL, GLFW_KEY_LEFT_CONTROL },
    { RFKeyCode::RCTRL, GLFW_KEY_RIGHT_CONTROL },
    { RFKeyCode::LALT, GLFW_KEY_LEFT_ALT },
    { RFKeyCode::RALT, GLFW_KEY_RIGHT_ALT }
};

GLFWInputLayer::GLFWInputLayer() : 
    window(nullptr)
{

}
GLFWInputLayer::GLFWInputLayer(GLFWwindow* window) :
	window(window)
{

}
GLFWInputLayer::~GLFWInputLayer()
{
    
}

void GLFWInputLayer::PreUpdate()
{
	InputLayer::PreUpdate();

	static glm::dvec2 previousMousePosition = mousePosition;

	glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
	// Ignore mouse delta on first frame
	if(TimeManager::GetCurrentFrame() > 0)
		mouseDelta = mousePosition - previousMousePosition;
	previousMousePosition = mousePosition;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	for(uint32_t i = 0; i < (uint32_t) MouseButtonCode::MAX; i++)
	{
        int mouseButton = GLFW_INPUT_MOUSE_BUTTON_MAP[(MouseButtonCode) i];

		// This button was just pressed this frame if it was not pressed last frame and is pressed this frame
		mouseButtonsPressedThisFrame[i] =
			!mouseButtonsDown[i] && glfwGetMouseButton(window, mouseButton) == GLFW_PRESS;
		// Update stored button state
		mouseButtonsDown[i] = glfwGetMouseButton(window, mouseButton) == GLFW_PRESS;
	}
	for(uint32_t i = 0; i < (uint32_t) RFKeyCode::MAX; i++)
	{
        int key = GLFW_INPUT_KEY_MAP[(RFKeyCode) i];

		// This key was just pressed this frame if it was not pressed last frame and is pressed this frame
		keysPressedThisFrame[i] =
			!keysDown[i] && glfwGetKey(window, key) == GLFW_PRESS;
		// Update stored button state
		keysDown[i] = glfwGetKey(window, key) == GLFW_PRESS;
	}
}
void GLFWInputLayer::PostUpdate()
{
	InputLayer::PostUpdate();
}