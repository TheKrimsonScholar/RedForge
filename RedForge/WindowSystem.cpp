#include "WindowSystem.h"

#include "GraphicsState.h"

void WindowSystem::Startup(const EngineStartupParams& params, World& world)
{
	static const uint32_t WIDTH = 800;
	static const uint32_t HEIGHT = 600;

	Window& window = world.GetResource<Window>();
	GraphicsState& graphicsState = world.GetResource<GraphicsState>();
	InputState& inputState = world.GetResource<InputState>();

    graphicsState.surface = params.surfaceOverride;

    window.isSurfaceOverridden = params.surfaceOverride != VK_NULL_HANDLE;

    if(!window.isSurfaceOverridden)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window.window = glfwCreateWindow(WIDTH, HEIGHT, "RedForge", nullptr, nullptr);
        glfwSetWindowUserPointer(window.window, this);
        glfwSetFramebufferSizeCallback(window.window, FramebufferResizeCallback);

        // Set input system to receive input from GLFW
        window.inputLayer = GLFWInputLayer(window.window);
        inputState.SetActiveInputLayer(params, &window.inputLayer);
    }
}
void WindowSystem::PostStartup(const EngineStartupParams& params, World& world)
{
    
}
void WindowSystem::Shutdown(const EngineShutdownParams& params, World& world)
{

}

void WindowSystem::Update(SystemContext<Window>& ctx, float deltaTime)
{

}

void WindowSystem::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    Window* windowState = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    windowState->wasFramebufferResized = true;
}