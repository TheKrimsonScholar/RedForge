#include "GLFWInputLayer.h"

#include <unordered_map>

#include "TimeManager.h"

static std::unordered_map<MouseButtonCode, int> GLFW_INPUT_MOUSE_BUTTON_MAP =
{
    { MouseButtonCode::Left, GLFW_MOUSE_BUTTON_LEFT },
    { MouseButtonCode::Middle, GLFW_MOUSE_BUTTON_MIDDLE },
    { MouseButtonCode::Right, GLFW_MOUSE_BUTTON_RIGHT }
};
static std::unordered_map<KeyCode, int> GLFW_INPUT_KEY_MAP =
{
    { KeyCode::A, GLFW_KEY_A },
    { KeyCode::B, GLFW_KEY_B },
    { KeyCode::C, GLFW_KEY_C },
    { KeyCode::D, GLFW_KEY_D },
    { KeyCode::E, GLFW_KEY_E },
    { KeyCode::F, GLFW_KEY_F },
    { KeyCode::G, GLFW_KEY_G },
    { KeyCode::H, GLFW_KEY_H },
    { KeyCode::I, GLFW_KEY_I },
    { KeyCode::J, GLFW_KEY_J },
    { KeyCode::K, GLFW_KEY_K },
    { KeyCode::L, GLFW_KEY_L },
    { KeyCode::M, GLFW_KEY_M },
    { KeyCode::N, GLFW_KEY_N },
    { KeyCode::O, GLFW_KEY_O },
    { KeyCode::P, GLFW_KEY_P },
    { KeyCode::Q, GLFW_KEY_Q },
    { KeyCode::R, GLFW_KEY_R },
    { KeyCode::S, GLFW_KEY_S },
    { KeyCode::T, GLFW_KEY_T },
    { KeyCode::U, GLFW_KEY_U },
    { KeyCode::V, GLFW_KEY_V },
    { KeyCode::W, GLFW_KEY_W },
    { KeyCode::X, GLFW_KEY_X },
    { KeyCode::Y, GLFW_KEY_Y },
    { KeyCode::Z, GLFW_KEY_Z },

    { KeyCode::LSHIFT, GLFW_KEY_LEFT_SHIFT },
    { KeyCode::RSHIFT, GLFW_KEY_RIGHT_SHIFT },
    { KeyCode::SPACE, GLFW_KEY_SPACE },
    { KeyCode::ESCAPE, GLFW_KEY_ESCAPE },
    { KeyCode::LCTRL, GLFW_KEY_LEFT_CONTROL },
    { KeyCode::RCTRL, GLFW_KEY_RIGHT_CONTROL },
    { KeyCode::LALT, GLFW_KEY_LEFT_ALT },
    { KeyCode::RALT, GLFW_KEY_RIGHT_ALT }
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
	for(uint32_t i = 0; i < (uint32_t) KeyCode::MAX; i++)
	{
        int key = GLFW_INPUT_KEY_MAP[(KeyCode) i];

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