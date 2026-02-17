#include "GraphicsSystem.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <map>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <chrono>
#include <format>
#include <filesystem>

#include "TimeManager.h"
#include "LevelManager.h"
#include "TransformComponent.h"
#include "ResourceManager.h"
#include "CameraManager.h"
#include "DebugManager.h"
#include "InputSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "rapidobj/rapidobj.hpp"

#define IMGUI_ENABLE_VIEWPORTS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_impl_glfw.h"

//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
//#include <windows.h>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
        func(instance, debugMessenger, pAllocator);
}

void GraphicsSystem::CreateVulkanInstance_PreStartup(GraphicsState& graphicsState)
{
    Instance = this;

    CreateVulkanInstance(graphicsState);
}
void GraphicsSystem::DestroyVulkanInstance_PostShutdown(const GraphicsState& graphicsState)
{
    DestroyVulkanInstance(graphicsState);
}

void GraphicsSystem::Startup(const EngineStartupParams& params, World& world)
{
    Instance = this;

	Window& window = world.GetResource<Window>();
	GraphicsState& graphicsState = world.GetResource<GraphicsState>();

    InitVulkan(window, graphicsState);

#ifdef _DEBUG
    InitImGui(window, graphicsState);
#endif
}
void GraphicsSystem::PostStartup(const EngineStartupParams& params, World& world)
{
    /* After all assets are loaded, create the global asset buffers */

	const Assets& assets = world.GetResource<Assets>();
	GraphicsState& graphicsState = world.GetResource<GraphicsState>();

    CreateGlobalArrayDescriptorSets(assets, graphicsState);

    UpdateMaterialsBuffer(assets, graphicsState);
}
void GraphicsSystem::Shutdown(const EngineShutdownParams& params, World& world)
{
    const Window& window = world.GetResource<Window>();
    const GraphicsState& graphicsState = world.GetResource<GraphicsState>();

#ifdef _DEBUG
    ImGui_ImplVulkan_Shutdown();
    if(!window.isSurfaceOverridden)
        ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif

    CleanupSwapChain(window, graphicsState);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(graphicsState.device, graphicsState.lightsBuffer[i], nullptr);
        vkFreeMemory(graphicsState.device, graphicsState.lightsBufferMemory[i], nullptr);

        vkDestroyBuffer(graphicsState.device, graphicsState.cameraUBOs[i], nullptr);
        vkFreeMemory(graphicsState.device, graphicsState.cameraUBOsMemory[i], nullptr);

        vkDestroyBuffer(graphicsState.device, graphicsState.instancesBuffer[i], nullptr);
        vkFreeMemory(graphicsState.device, graphicsState.instancesBufferMemory[i], nullptr);

        vkDestroyBuffer(graphicsState.device, graphicsState.transformBuffer[i], nullptr);
        vkFreeMemory(graphicsState.device, graphicsState.transformBufferMemory[i], nullptr);

        vkDestroyBuffer(graphicsState.device, graphicsState.materialsBuffer[i], nullptr);
        vkFreeMemory(graphicsState.device, graphicsState.materialsBufferMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(graphicsState.device, graphicsState.imguiDescriptorPool, nullptr);
    vkDestroyDescriptorPool(graphicsState.device, graphicsState.descriptorPool, nullptr);

    vkDestroyDescriptorSetLayout(graphicsState.device, graphicsState.descriptorSetLayout, nullptr);

    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(graphicsState.device, graphicsState.imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(graphicsState.device, graphicsState.renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(graphicsState.device, graphicsState.inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(graphicsState.device, graphicsState.commandPool, nullptr);

    vkDestroyPipeline(graphicsState.device, graphicsState.debugGraphicsPipeline, nullptr);
    vkDestroyPipeline(graphicsState.device, graphicsState.skyboxGraphicsPipeline, nullptr);
    vkDestroyPipeline(graphicsState.device, graphicsState.graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(graphicsState.device, graphicsState.debugPipelineLayout, nullptr);
    vkDestroyPipelineLayout(graphicsState.device, graphicsState.skyboxPipelineLayout, nullptr);
    vkDestroyPipelineLayout(graphicsState.device, graphicsState.pipelineLayout, nullptr);
    vkDestroyRenderPass(graphicsState.device, graphicsState.renderPass, nullptr);

    vkDestroyDevice(graphicsState.device, nullptr);

    if(enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(graphicsState.vulkanInstance, graphicsState.debugMessenger, nullptr);

    if(!window.isSurfaceOverridden)
        vkDestroySurfaceKHR(graphicsState.vulkanInstance, graphicsState.surface, nullptr);
    if(params.shouldDestroyVulkanInstance)
        DestroyVulkanInstance(graphicsState);

    glfwDestroyWindow(window.window);

    glfwTerminate();
}

void GraphicsSystem::Update(LocalSystemContext& ctx, float deltaTime)
{
    Window& window = ctx.GetResource<Window>();
    GraphicsState& graphicsState = ctx.GetResource<GraphicsState>();
	const Cameras& cameras = ctx.GetResource<const Cameras>();
	const Assets& assets = ctx.GetResource<const Assets>();
	const Time& time = ctx.GetResource<const Time>();

    // Wait until the previous frame is finished
    vkWaitForFences(graphicsState.device, 1, &graphicsState.inFlightFences[graphicsState.currentFrame], VK_TRUE, UINT64_MAX);

    // Acquire an image from the swap chain
    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(graphicsState.device, window.swapChain, UINT64_MAX, graphicsState.imageAvailableSemaphores[graphicsState.currentFrame], VK_NULL_HANDLE, &imageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain(window, graphicsState);
        return;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to acquire swap chain image!");

    // Only reset the fence if we are submitting work
    vkResetFences(graphicsState.device, 1, &graphicsState.inFlightFences[graphicsState.currentFrame]);

    // Record command buffer
    vkResetCommandBuffer(graphicsState.commandBuffers[graphicsState.currentFrame], 0);
    RecordCommandBuffer(ctx, graphicsState.commandBuffers[graphicsState.currentFrame], imageIndex);

    // Update UBO data
    UpdateUniformBuffer(graphicsState, cameras, ctx.GetContext<const TransformComponent, const CameraComponent>(), graphicsState.currentFrame);
    UpdateGlobalArrays(graphicsState.currentFrame);

    /* Submit command buffer */

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { graphicsState.imageAvailableSemaphores[graphicsState.currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &graphicsState.commandBuffers[graphicsState.currentFrame];

    VkSemaphore signalSemaphores[] = { graphicsState.renderFinishedSemaphores[graphicsState.currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(graphicsState.graphicsQueue, 1, &submitInfo, graphicsState.inFlightFences[graphicsState.currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer!");

    /* Present */

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { window.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(graphicsState.presentQueue, &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasFramebufferResized)
    {
        window.wasFramebufferResized = false;
        RecreateSwapChain(window, graphicsState);
    }
    else if(result != VK_SUCCESS)
        throw std::runtime_error("Failed to present swap chain image!");

    graphicsState.currentFrame = (graphicsState.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VKAPI_ATTR VkBool32 VKAPI_CALL GraphicsSystem::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void GraphicsSystem::InitVulkan(Window& window, GraphicsState& graphicsState)
{
    CreateVulkanInstance(graphicsState);
    SetupDebugMessenger(graphicsState);
    if(!window.isSurfaceOverridden)
        CreateSurface(window, graphicsState);
    SelectPhysicalDevice(window, graphicsState);
    CreateLogicalDevice(window, graphicsState);
    //if(!shouldRenderOffscreen)
    //{
        CreateSwapChain(window, graphicsState);
        CreateImageViews(window, graphicsState);
    //}
    CreateRenderPass(window, graphicsState);
    CreateDescriptorSetLayout(graphicsState);
    CreateGraphicsPipeline(window, graphicsState);
    CreateSkyboxGraphicsPipeline(window, graphicsState);
    CreateDebugGraphicsPipeline(window, graphicsState);
    CreateCommandPool(window, graphicsState);
    CreateColorResources(window, graphicsState);
    CreateDepthResources(window, graphicsState);
    //if(!shouldRenderOffscreen)
        CreateFramebuffers(window, graphicsState);
    CreateUniformBuffers(graphicsState);
    CreateGlobalBuffers(graphicsState);
    CreateDescriptorPool(graphicsState);
    CreateCommandBuffers(graphicsState);
    CreateSyncObjects(graphicsState);

    /*if(shouldRenderOffscreen)
    {
        CreateExternalRenderSyncObjects();
        CreateExternalRenderResources();
    }*/
}
void GraphicsSystem::InitImGui(const Window& window, GraphicsState& graphicsState)
{
    CreateImGuiDescriptorPool(graphicsState);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    if(!window.isSurfaceOverridden)
        ImGui_ImplGlfw_InitForVulkan(window.window, true);

    QueueFamilyIndices indices = FindQueueFamilies(window, graphicsState, graphicsState.physicalDevice);

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = graphicsState.vulkanInstance;
    initInfo.PhysicalDevice = graphicsState.physicalDevice;
    initInfo.Device = graphicsState.device;
    initInfo.QueueFamily = indices.graphicsFamily.value();
    initInfo.Queue = graphicsState.graphicsQueue;
    initInfo.PipelineCache = nullptr;
	initInfo.RenderPass = graphicsState.renderPass;
    initInfo.DescriptorPool = graphicsState.imguiDescriptorPool;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = 2;
    initInfo.MSAASamples = graphicsState.msaaSamples;
    initInfo.CheckVkResultFn = nullptr;

    ImGui_ImplVulkan_Init(&initInfo);

    VkCommandBuffer commandBuffer = graphicsState.BeginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture();
    graphicsState.EndSingleTimeCommands(commandBuffer);
    ImGui_ImplVulkan_DestroyFontsTexture();
}

void GraphicsSystem::CreateVulkanInstance(GraphicsState& graphicsState)
{
    // Skip if Vulkan instance has already been created
    if(graphicsState.vulkanInstance)
        return;

    if(enableValidationLayers && !CheckValidationLayerSupport())
        throw std::runtime_error("Validation layers requested, but not available!");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "RedForge Game";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "RedForge";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &graphicsState.vulkanInstance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance!");
}
bool GraphicsSystem::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            return false;
    }

    return true;
}
std::vector<const char*> GraphicsSystem::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    for(const char* e : instanceExtensions)
        extensions.push_back(e);

    if(enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

void GraphicsSystem::SetupDebugMessenger(GraphicsState& graphicsState)
{
    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(graphicsState.vulkanInstance, &createInfo, nullptr, &graphicsState.debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug messenger!");
}
void GraphicsSystem::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

void GraphicsSystem::CreateSurface(const Window& window, GraphicsState& graphicsState)
{
    if(glfwCreateWindowSurface(graphicsState.vulkanInstance, window.window, nullptr, &graphicsState.surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface!");
}

void GraphicsSystem::SelectPhysicalDevice(const Window& window, GraphicsState& graphicsState)
{
    // Check if there are any valid devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(graphicsState.vulkanInstance, &deviceCount, nullptr);
    if(deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");

    // Get all valid devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(graphicsState.vulkanInstance, &deviceCount, devices.data());

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> deviceScores;
    // Rate each device's suitability
    for(const auto& device : devices)
    {
        int score = RateDeviceSuitability(window, graphicsState, device);
        deviceScores.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if(deviceScores.rbegin()->first > 0)
    {
        graphicsState.physicalDevice = deviceScores.rbegin()->second;
        graphicsState.msaaSamples = GetMaxUsableSampleCount(graphicsState);
    }
    else
        throw std::runtime_error("Failed to find a suitable GPU!");
}
int GraphicsSystem::RateDeviceSuitability(const Window& window, const GraphicsState& graphicsState, VkPhysicalDevice device)
{
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Greatly prioritize discrete GPUs
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;
    // Prioritize higher quality textures
    score += deviceProperties.limits.maxImageDimension2D;
    // Require geometry shaders
    if(!deviceFeatures.geometryShader)
        return 0;
    // Require anisotropic filtering
    if(!deviceFeatures.samplerAnisotropy)
        return 0;
    // Require wireframe support
    if(!deviceFeatures.fillModeNonSolid)
        return 0;

    // Queue family checks
    QueueFamilyIndices indices = FindQueueFamilies(window, graphicsState, device);
    if(!indices.IsComplete())
        return 0;

    // Extension checks
    if(!CheckDeviceExtensionSupport(device))
        return 0;

    // Swap chain checks - ignore if rendering offscreen
    if(!window.isSurfaceOverridden)
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(graphicsState, device);
        bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        if(!swapChainAdequate)
            return 0;
    }

    return score;
}
QueueFamilyIndices GraphicsSystem::FindQueueFamilies(const Window& window, const GraphicsState& graphicsState, VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // Find at least one of each queue family
    int i = 0;
    for(const auto& queueFamily : queueFamilies)
    {
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        // Check if the queue family supports presenting to the window surface (If rendering offscreen, don't need present support)
        if(!window.isSurfaceOverridden)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, graphicsState.surface, &presentSupport);
            if(presentSupport)
                indices.presentFamily = i;
        }
        else
            indices.presentFamily = 0;

        // Early exit if we find a suitable queue family
        if(indices.IsComplete())
            break;

        i++;
    }

    return indices;
}
bool GraphicsSystem::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    // Check if all required extensions are available
    for(const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}
SwapChainSupportDetails GraphicsSystem::QuerySwapChainSupport(const GraphicsState& graphicsState, VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    // Get the basic surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, graphicsState.surface, &details.capabilities);

    // Get the suppoprted surface formats if there are any
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, graphicsState.surface, &formatCount, nullptr);
    if(formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, graphicsState.surface, &formatCount, details.formats.data());
    }

    // Get the supported presentation modes if there are any
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, graphicsState.surface, &presentModeCount, nullptr);
    if(presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, graphicsState.surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
VkSampleCountFlagBits GraphicsSystem::GetMaxUsableSampleCount(const GraphicsState& graphicsState)
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(graphicsState.physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if(counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
    if(counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
    if(counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
    if(counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
    if(counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
    if(counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

    return VK_SAMPLE_COUNT_1_BIT;
}

void GraphicsSystem::CreateLogicalDevice(const Window& window, GraphicsState& graphicsState)
{
    QueueFamilyIndices indices = FindQueueFamilies(window, graphicsState, graphicsState.physicalDevice);

    // Create all queues
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for(uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
    indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    indexingFeatures.runtimeDescriptorArray = VK_TRUE;
    indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
    indexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
    indexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    indexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;

    /* Create the logical device */

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.pNext = &indexingFeatures;

    // Deprecated parameters; still set for compatibility with older implementations
    if(enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    if(vkCreateDevice(graphicsState.physicalDevice, &createInfo, nullptr, &graphicsState.device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device!");

    // Get queues
    vkGetDeviceQueue(graphicsState.device, indices.graphicsFamily.value(), 0, &graphicsState.graphicsQueue);
    vkGetDeviceQueue(graphicsState.device, indices.presentFamily.value(), 0, &graphicsState.presentQueue);
}

void GraphicsSystem::CreateSwapChain(Window& window, const GraphicsState& graphicsState)
{
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(graphicsState, graphicsState.physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(window, swapChainSupport.capabilities);

    // How many images should be in the swap chain?
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    /* Create Swap Chain */

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = graphicsState.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(window, graphicsState, graphicsState.physicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    // How will swap chain images be used across multiple queue families?
    if(indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(graphicsState.device, &createInfo, nullptr, &window.swapChain);
    if(result != VK_SUCCESS)
        throw std::runtime_error("Failed to create swap chain!");

    // Retrieve swap chain images
    vkGetSwapchainImagesKHR(graphicsState.device, window.swapChain, &imageCount, nullptr);
    window.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(graphicsState.device, window.swapChain, &imageCount, window.swapChainImages.data());

    window.swapChainImageFormat = surfaceFormat.format;
    window.swapChainExtent = extent;
}
VkSurfaceFormatKHR GraphicsSystem::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    // Look for preferred format
    for(const auto& availableFormat : availableFormats)
    {
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }

    // Otherwise use the first one available
    return availableFormats[0];
}
VkPresentModeKHR GraphicsSystem::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    // Look for preferred present mode
    for(const auto& availablePresentMode : availablePresentModes)
    {
        if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }

    // Fallback; always available
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D GraphicsSystem::ChooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities)
{
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    else
    {
        int width, height;
        glfwGetFramebufferSize(window.window, &width, &height);

        VkExtent2D actualExtent =
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void GraphicsSystem::CreateImageViews(Window& window, const GraphicsState& graphicsState)
{
    window.swapChainImageViews.resize(window.swapChainImages.size());
    
    // Create image view for each swap chain image
    for(size_t i = 0; i < window.swapChainImages.size(); i++)
        window.swapChainImageViews[i] = CreateImageView(graphicsState, window.swapChainImages[i], window.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void GraphicsSystem::CreateRenderPass(const Window& window, GraphicsState& graphicsState)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = window.swapChainImageFormat;
    colorAttachment.samples = graphicsState.msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat(graphicsState);
    depthAttachment.samples = graphicsState.msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = window.swapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if(vkCreateRenderPass(graphicsState.device, &renderPassInfo, nullptr, &graphicsState.renderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create render pass!");
}

void GraphicsSystem::CreateDescriptorSetLayout(GraphicsState& graphicsState)
{
    VkDescriptorSetLayoutBinding materialsArrayBinding = {};
    materialsArrayBinding.binding = 0;
    materialsArrayBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    materialsArrayBinding.descriptorCount = 1;
    materialsArrayBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    materialsArrayBinding.pImmutableSamplers = nullptr;
    
    VkDescriptorSetLayoutBinding transformsArrayBinding{};
    transformsArrayBinding.binding = 1;
    transformsArrayBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    transformsArrayBinding.descriptorCount = 1;
    transformsArrayBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    transformsArrayBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding instancesArrayBinding{};
    instancesArrayBinding.binding = 2;
    instancesArrayBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    instancesArrayBinding.descriptorCount = 1;
    instancesArrayBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    instancesArrayBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding skyboxTextureCubeBinding = {};
    skyboxTextureCubeBinding.binding = 3;
    skyboxTextureCubeBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    skyboxTextureCubeBinding.descriptorCount = 1;
    skyboxTextureCubeBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    skyboxTextureCubeBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding cameraUBOBinding{};
    cameraUBOBinding.binding = 4;
    cameraUBOBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraUBOBinding.descriptorCount = 1;
    cameraUBOBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    cameraUBOBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding lightsArrayBinding{};
    lightsArrayBinding.binding = 5;
    lightsArrayBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    lightsArrayBinding.descriptorCount = 1;
    lightsArrayBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    lightsArrayBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding texturesArrayBinding = {};
    texturesArrayBinding.binding = 6;
    texturesArrayBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    texturesArrayBinding.descriptorCount = MAX_TEXTURES;
    texturesArrayBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    texturesArrayBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 7> bindings = { materialsArrayBinding, transformsArrayBinding, instancesArrayBinding, skyboxTextureCubeBinding, cameraUBOBinding, lightsArrayBinding, texturesArrayBinding };
    
    VkDescriptorBindingFlags bindingFlags[7] = 
    {
        0,
        0,
        0,
        0,
        0,
        0,
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
    };

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{};
    bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingFlagsInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    bindingFlagsInfo.pBindingFlags = bindingFlags;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    layoutInfo.pNext = &bindingFlagsInfo;

    if(vkCreateDescriptorSetLayout(graphicsState.device, &layoutInfo, nullptr, &graphicsState.descriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor set layout!");
}
void GraphicsSystem::CreateGraphicsPipeline(const Window& window, GraphicsState& graphicsState)
{
    auto vertShaderCode = File::ReadFile(GetEngineAssetsPath().append(L"shaders/vert.spv"));
    auto fragShaderCode = File::ReadFile(GetEngineAssetsPath().append(L"shaders/frag.spv"));

    VkShaderModule vertShaderModule = CreateShaderModule(graphicsState, vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(graphicsState, fragShaderCode);

    /* Describe the programmable stages of the pipeline */

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    /* Fixed-function stages */

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) window.swapChainExtent.width;
    viewport.height = (float) window.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = window.swapChainExtent;

    /* Viewport and scissor rectangle will have dynamic state */

    std::vector<VkDynamicState> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = graphicsState.msaaSamples;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    /* Pipeline layout */

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &graphicsState.descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if(vkCreatePipelineLayout(graphicsState.device, &pipelineLayoutInfo, nullptr, &graphicsState.pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout!");

    /* Create pipeline */

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = graphicsState.pipelineLayout;
    pipelineInfo.renderPass = graphicsState.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if(vkCreateGraphicsPipelines(graphicsState.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsState.graphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create graphics pipeline!");

    vkDestroyShaderModule(graphicsState.device, fragShaderModule, nullptr);
    vkDestroyShaderModule(graphicsState.device, vertShaderModule, nullptr);
}
void GraphicsSystem::CreateSkyboxGraphicsPipeline(const Window& window, GraphicsState& graphicsState)
{
    auto vertShaderCode = File::ReadFile(GetEngineAssetsPath().append(L"shaders/VSSkybox.spv"));
    auto fragShaderCode = File::ReadFile(GetEngineAssetsPath().append(L"shaders/FSSkybox.spv"));

    VkShaderModule vertShaderModule = CreateShaderModule(graphicsState, vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(graphicsState, fragShaderCode);

    /* Describe the programmable stages of the pipeline */

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    /* Fixed-function stages */

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) window.swapChainExtent.width;
    viewport.height = (float) window.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = window.swapChainExtent;

    /* Viewport and scissor rectangle will have dynamic state */

    std::vector<VkDynamicState> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = graphicsState.msaaSamples;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;

    /* Pipeline layout */

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &graphicsState.descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if(vkCreatePipelineLayout(graphicsState.device, &pipelineLayoutInfo, nullptr, &graphicsState.skyboxPipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout!");

    /* Create pipeline */

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = graphicsState.skyboxPipelineLayout;
    pipelineInfo.renderPass = graphicsState.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if(vkCreateGraphicsPipelines(graphicsState.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsState.skyboxGraphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create graphics pipeline!");

    vkDestroyShaderModule(graphicsState.device, fragShaderModule, nullptr);
    vkDestroyShaderModule(graphicsState.device, vertShaderModule, nullptr);
}
void GraphicsSystem::CreateDebugGraphicsPipeline(const Window& window, GraphicsState& graphicsState)
{
    auto vertShaderCode = File::ReadFile(GetEngineAssetsPath().append(L"shaders/vert.spv"));
    auto fragShaderCode = File::ReadFile(GetEngineAssetsPath().append(L"shaders/frag.spv"));

    VkShaderModule vertShaderModule = CreateShaderModule(graphicsState, vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(graphicsState, fragShaderCode);

    /* Describe the programmable stages of the pipeline */

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    /* Fixed-function stages */

    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) window.swapChainExtent.width;
    viewport.height = (float) window.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = window.swapChainExtent;

    /* Viewport and scissor rectangle will have dynamic state */

    std::vector<VkDynamicState> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = graphicsState.msaaSamples;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    /* Pipeline layout */

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &graphicsState.descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if(vkCreatePipelineLayout(graphicsState.device, &pipelineLayoutInfo, nullptr, &graphicsState.debugPipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout!");

    /* Create pipeline */

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = graphicsState.debugPipelineLayout;
    pipelineInfo.renderPass = graphicsState.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if(vkCreateGraphicsPipelines(graphicsState.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsState.debugGraphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create graphics pipeline!");

    vkDestroyShaderModule(graphicsState.device, fragShaderModule, nullptr);
    vkDestroyShaderModule(graphicsState.device, vertShaderModule, nullptr);
}
VkShaderModule GraphicsSystem::CreateShaderModule(const GraphicsState& graphicsState, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(graphicsState.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module!");

    return shaderModule;
}

void GraphicsSystem::CreateFramebuffers(Window& window, const GraphicsState& graphicsState)
{
    window.swapChainFramebuffers.resize(window.swapChainImageViews.size());

    for(size_t i = 0; i < window.swapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 3> attachments =
        {
            graphicsState.colorImageView,
            graphicsState.depthImageView,
            window.swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = graphicsState.renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = window.swapChainExtent.width;
        framebufferInfo.height = window.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(graphicsState.device, &framebufferInfo, nullptr, &window.swapChainFramebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create framebuffer!");
    }
}

void GraphicsSystem::CreateCommandPool(const Window& window, GraphicsState& graphicsState)
{
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(window, graphicsState, graphicsState.physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if(vkCreateCommandPool(graphicsState.device, &poolInfo, nullptr, &graphicsState.commandPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool!");
}

void GraphicsSystem::CreateColorResources(const Window& window, GraphicsState& graphicsState)
{
    VkFormat colorFormat = window.swapChainImageFormat;

    graphicsState.CreateImage(
        window.GetWidth(), window.GetHeight(), 1,
        graphicsState.msaaSamples,
        colorFormat, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        graphicsState.colorImage,
        graphicsState.colorImageMemory);
    graphicsState.colorImageView = CreateImageView(graphicsState, graphicsState.colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}
void GraphicsSystem::CreateDepthResources(const Window& window, GraphicsState& graphicsState)
{
    VkFormat depthFormat = FindDepthFormat(graphicsState);

    graphicsState.CreateImage(
        window.GetWidth(), window.GetHeight(), 1,
        graphicsState.msaaSamples,
        depthFormat, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        graphicsState.depthImage,
        graphicsState.depthImageMemory);
    graphicsState.depthImageView = CreateImageView(graphicsState, graphicsState.depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}
VkFormat GraphicsSystem::FindDepthFormat(const GraphicsState& graphicsState)
{
    return FindSupportedFormat(
        graphicsState,
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}
VkFormat GraphicsSystem::FindSupportedFormat(const GraphicsState& graphicsState, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for(VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(graphicsState.physicalDevice, format, &props);

        if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    throw std::runtime_error("Failed to find supported format!");
}
bool GraphicsSystem::HasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void GraphicsSystem::CreateGlobalBuffers(GraphicsState& graphicsState)
{
    /* Materials array */
    {
        VkDeviceSize bufferSize = sizeof(MaterialData) * MAX_MATERIALS;

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            graphicsState.CreateBuffer(
                bufferSize, 
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                graphicsState.materialsBuffer[i],
                graphicsState.materialsBufferMemory[i]);

            vkMapMemory(graphicsState.device, graphicsState.materialsBufferMemory[i], 0, bufferSize, 0, &graphicsState.materialsBufferMapped[i]);
        }
    }

    /* Transforms array */
    {
        VkDeviceSize bufferSize = sizeof(glm::mat4) * MAX_INSTANCES;

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            graphicsState.CreateBuffer(
                bufferSize, 
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                graphicsState.transformBuffer[i],
                graphicsState.transformBufferMemory[i]);

            vkMapMemory(graphicsState.device, graphicsState.transformBufferMemory[i], 0, bufferSize, 0, &graphicsState.transformBufferMapped[i]);
        }
    }

    /* Instances array */
    {
        VkDeviceSize bufferSize = sizeof(InstanceData) * MAX_INSTANCES;

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            graphicsState.CreateBuffer(
                bufferSize, 
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                graphicsState.instancesBuffer[i],
                graphicsState.instancesBufferMemory[i]);

            vkMapMemory(graphicsState.device, graphicsState.instancesBufferMemory[i], 0, bufferSize, 0, &graphicsState.instancesBufferMapped[i]);
        }
    }

    /* Lights array */
    {
        VkDeviceSize bufferSize = sizeof(LightBufferData);

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            graphicsState.CreateBuffer(
                bufferSize, 
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                graphicsState.lightsBuffer[i],
                graphicsState.lightsBufferMemory[i]);

            vkMapMemory(graphicsState.device, graphicsState.lightsBufferMemory[i], 0, bufferSize, 0, &graphicsState.lightsBufferMapped[i]);
        }
    }
}
void GraphicsSystem::CreateGlobalArrayDescriptorSets(const Assets& assets, GraphicsState& graphicsState)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, graphicsState.descriptorSetLayout);
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = graphicsState.descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    if(vkAllocateDescriptorSets(graphicsState.device, &allocInfo, graphicsState.descriptorSets) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate descriptor sets!");
    
    for(const Texture* texture : assets.GetTextures())
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture->textureImageView;
        imageInfo.sampler = texture->textureSampler;

        graphicsState.texturesBufferData.push_back(imageInfo);
    }

    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo materialsBufferInfo{};
        materialsBufferInfo.buffer = graphicsState.materialsBuffer[i];
        materialsBufferInfo.offset = 0;
        materialsBufferInfo.range = VK_WHOLE_SIZE;

        VkDescriptorBufferInfo transformsBufferInfo{};
        transformsBufferInfo.buffer = graphicsState.transformBuffer[i];
        transformsBufferInfo.offset = 0;
        transformsBufferInfo.range = VK_WHOLE_SIZE;

        VkDescriptorBufferInfo instancesBufferInfo{};
        instancesBufferInfo.buffer = graphicsState.instancesBuffer[i];
        instancesBufferInfo.offset = 0;
        instancesBufferInfo.range = VK_WHOLE_SIZE;

        VkDescriptorImageInfo skyboxImageInfo{};
        skyboxImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        skyboxImageInfo.imageView = graphicsState.skyboxTextureCube->textureImageView;
        skyboxImageInfo.sampler = graphicsState.skyboxTextureCube->sampler;

        VkDescriptorBufferInfo cameraUBOInfo{};
        cameraUBOInfo.buffer = graphicsState.cameraUBOs[i];
        cameraUBOInfo.offset = 0;
        cameraUBOInfo.range = sizeof(UniformBufferObject);

        VkDescriptorBufferInfo lightsBufferInfo{};
        lightsBufferInfo.buffer = graphicsState.lightsBuffer[i];
        lightsBufferInfo.offset = 0;
        lightsBufferInfo.range = VK_WHOLE_SIZE;

        std::array<VkWriteDescriptorSet, 7> descriptorWrites{};

        // Materials array
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = graphicsState.descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &materialsBufferInfo;

        // Transforms array
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = graphicsState.descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &transformsBufferInfo;

        // Instances array
        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = graphicsState.descriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &instancesBufferInfo;

        // Skybox texture
        descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[3].dstSet = graphicsState.descriptorSets[i];
        descriptorWrites[3].dstBinding = 3;
        descriptorWrites[3].dstArrayElement = 0;
        descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[3].descriptorCount = 1;
        descriptorWrites[3].pImageInfo = &skyboxImageInfo;

        // Camera UBO
        descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[4].dstSet = graphicsState.descriptorSets[i];
        descriptorWrites[4].dstBinding = 4;
        descriptorWrites[4].dstArrayElement = 0;
        descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[4].descriptorCount = 1;
        descriptorWrites[4].pBufferInfo = &cameraUBOInfo;

        // Lights array
        descriptorWrites[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[5].dstSet = graphicsState.descriptorSets[i];
        descriptorWrites[5].dstBinding = 5;
        descriptorWrites[5].dstArrayElement = 0;
        descriptorWrites[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[5].descriptorCount = 1;
        descriptorWrites[5].pBufferInfo = &lightsBufferInfo;
        
        // Textures array
        descriptorWrites[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[6].dstSet = graphicsState.descriptorSets[i];
        descriptorWrites[6].dstBinding = 6;
        descriptorWrites[6].dstArrayElement = 0;
        descriptorWrites[6].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[6].descriptorCount = static_cast<uint32_t>(graphicsState.texturesBufferData.size());
        descriptorWrites[6].pImageInfo = graphicsState.texturesBufferData.data();
        
        vkUpdateDescriptorSets(graphicsState.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}
void GraphicsSystem::UpdateMaterialsBuffer(const Assets& assets, GraphicsState& graphicsState)
{
    graphicsState.materialsBufferData.clear();
    for(const Material* material : assets.GetMaterials())
    {
        MaterialData materialData{};
        materialData.albedoIndex = material->albedoTexture->index;
        if(material->normalsTexture)
            materialData.normalsIndex = material->normalsTexture->index;
        if(material->roughnessTexture)
            materialData.roughnessIndex = material->roughnessTexture->index;
        if(material->metalnessTexture)
            materialData.metalnessIndex = material->metalnessTexture->index;

        graphicsState.materialsBufferData.push_back(materialData);
    }

    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        memcpy(graphicsState.materialsBufferMapped[i], graphicsState.materialsBufferData.data(), sizeof(MaterialData) * graphicsState.materialsBufferData.size());
}
void GraphicsSystem::UpdateGlobalArrays(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    /*for(Entity e = 0; e < EntityManager::GetLastEntity(); e++)
		if(EntityManager::HasComponent<TransformComponent>(e))
		    EntityManager::GetComponent<TransformComponent>(e).degrees += 0.1f;*/
}

void GraphicsSystem::CreateTextureImageView(const GraphicsState& graphicsState, Texture* texture)
{
    texture->textureImageView = CreateImageView(graphicsState, texture->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture->mipLevels);
}

VkImageView GraphicsSystem::CreateImageView(const GraphicsState& graphicsState, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, 
    VkImageViewType viewType, uint32_t layerCount)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layerCount;

    VkImageView imageView;
    if(vkCreateImageView(graphicsState.device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("Failed to create texture image view!");

    return imageView;
}

void GraphicsSystem::CreateUniformBuffers(GraphicsState& graphicsState)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        graphicsState.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            graphicsState.cameraUBOs[i],
            graphicsState.cameraUBOsMemory[i]);

        vkMapMemory(graphicsState.device, graphicsState.cameraUBOsMemory[i], 0, bufferSize, 0, &graphicsState.cameraUBOsMapped[i]);
    }
}
void GraphicsSystem::CreateDescriptorPool(GraphicsState& graphicsState)
{
    std::array<VkDescriptorPoolSize, 3> poolSizes{};
    // Textures array
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = MAX_TEXTURES * static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    // Materials array, Transforms array, Instances array
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 3;
    // Camera UBO
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2; // Scene data, Camera data
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

    if(vkCreateDescriptorPool(graphicsState.device, &poolInfo, nullptr, &graphicsState.descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor pool!");
}
void GraphicsSystem::CreateImGuiDescriptorPool(GraphicsState& graphicsState)
{
    VkDescriptorPoolSize poolSizes[] = 
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
    poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
    poolInfo.pPoolSizes = poolSizes;

    vkCreateDescriptorPool(graphicsState.device, &poolInfo, nullptr, &graphicsState.imguiDescriptorPool);
}

uint32_t GraphicsSystem::FindMemoryType(const GraphicsState& graphicsState, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(graphicsState.physicalDevice, &memProperties);
    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if(typeFilter & (1 << i) 
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    throw std::runtime_error("Failed to find suitable memory type!");
}
void GraphicsSystem::CreateCommandBuffers(GraphicsState& graphicsState)
{
    graphicsState.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = graphicsState.commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) MAX_FRAMES_IN_FLIGHT;

    if(vkAllocateCommandBuffers(graphicsState.device, &allocInfo, graphicsState.commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffers!");
}

void GraphicsSystem::CreateSyncObjects(GraphicsState& graphicsState)
{
    graphicsState.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    graphicsState.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    graphicsState.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start in signaled state so first frame doesn't need to wait

    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        if(vkCreateSemaphore(graphicsState.device, &semaphoreInfo, nullptr, &graphicsState.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(graphicsState.device, &semaphoreInfo, nullptr, &graphicsState.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(graphicsState.device, &fenceInfo, nullptr, &graphicsState.inFlightFences[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create semaphores!");
}

void GraphicsSystem::CleanupSwapChain(const Window& window, const GraphicsState& graphicsState)
{
    vkDestroyImageView(graphicsState.device, graphicsState.colorImageView, nullptr);
    vkDestroyImage(graphicsState.device, graphicsState.colorImage, nullptr);
    vkFreeMemory(graphicsState.device, graphicsState.colorImageMemory, nullptr);

    vkDestroyImageView(graphicsState.device, graphicsState.depthImageView, nullptr);
    vkDestroyImage(graphicsState.device, graphicsState.depthImage, nullptr);
    vkFreeMemory(graphicsState.device, graphicsState.depthImageMemory, nullptr);

    for(auto framebuffer : window.swapChainFramebuffers)
        vkDestroyFramebuffer(graphicsState.device, framebuffer, nullptr);

    for(auto imageView : window.swapChainImageViews)
        vkDestroyImageView(graphicsState.device, imageView, nullptr);

    vkDestroySwapchainKHR(graphicsState.device, window.swapChain, nullptr);
}

void GraphicsSystem::DestroyVulkanInstance(const GraphicsState& graphicsState)
{
    vkDestroyInstance(graphicsState.vulkanInstance, nullptr);
}

void GraphicsSystem::RecreateSwapChain(Window& window, GraphicsState& graphicsState)
{
    if(!window.isSurfaceOverridden)
    {
        int width = 0, height = 0;
        // Pause while window is minimized
        while(width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window.window, &width, &height);
            glfwWaitEvents();
        }
    }

    vkDeviceWaitIdle(graphicsState.device);

    CleanupSwapChain(window, graphicsState);

    CreateSwapChain(window, graphicsState);
    CreateImageViews(window, graphicsState);
    CreateColorResources(window, graphicsState);
    CreateDepthResources(window, graphicsState);
    //if(!Instance->isSurfaceOverridden)
        CreateFramebuffers(window, graphicsState);
}

void GraphicsSystem::RecordCommandBuffer(LocalSystemContext& ctx, VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    const Window& window = ctx.GetResource<const Window>();
    GraphicsState& graphicsState = ctx.GetResource<GraphicsState>();
    const Assets& assets = ctx.GetResource<const Assets>();
    const Time& time = ctx.GetResource<const Time>();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = graphicsState.renderPass;
    renderPassInfo.framebuffer = window.swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = window.swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsState.graphicsPipeline);

    /* Set the viewport and scissor state in the command buffer (dynamic state) */

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(window.GetWidth());
    viewport.height = static_cast<float>(window.GetHeight());
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = window.swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(
        commandBuffer, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        graphicsState.pipelineLayout,
        0, 
        1, 
        &graphicsState.descriptorSets[graphicsState.currentFrame],
        0, 
        nullptr);
    
    /* Gather renderer instances from Entity Manager */
    /* Also gather lights */
    // TODO - Move to UpdateGlobalBuffers()?
    // TODO - Make instanceData persist through frames?
    graphicsState.instanceData.clear();
    std::unordered_map<Entity, uint32_t> entityInstanceIndices;
    std::vector<LightData> lightsData;
    ctx.ForEachEntityInLevel([this, &ctx, &graphicsState, &assets, &entityInstanceIndices, &lightsData](const Entity& entity)
        {
            if(ctx.HasComponent<MeshRendererComponent>(entity) && ctx.HasComponent<TransformComponent>(entity))
            {
                Entity parent = ctx.GetEntityParent(entity);

                const MeshRendererComponent& renderer = ctx.GetComponent<const MeshRendererComponent>(entity);
                const TransformComponent& transform = ctx.GetComponent<const TransformComponent>(entity);

                InstanceData instance{};
                instance.rendererIndex = graphicsState.instanceData.size();
                instance.meshIndex = assets.GetMesh(renderer.mesh.identifier).index;
                instance.materialIndex = assets.GetMaterial(renderer.material.identifier).index;

                entityInstanceIndices.emplace(entity, graphicsState.instanceData.size());
                graphicsState.instanceData.push_back(instance);

                // Combine this transform with its parent's
                // Because we know the parent has already been processed (by the order of the LevelManager's entity hierarchy), this gives the world transform of the current entity
                glm::mat4 parentMatrix = parent.IsValid() ? graphicsState.modelMatrices[entityInstanceIndices[parent]] : glm::mat4(1.0f);
                graphicsState.modelMatrices[instance.rendererIndex] = parentMatrix * transform.GetMatrix();
            }

            if(ctx.HasComponent<LightComponent>(entity))
            {
                const LightComponent& light = ctx.GetComponent<const LightComponent>(entity);

                LightData lightData{};
                lightData.direction = light.direction;
                lightData.lightType = light.lightType;
                lightData.color = light.color;
                lightData.intensity = light.intensity;
                lightData.location = light.location;
                lightData.range = light.range;
                lightData.spotInnerAngle = light.spotInnerAngle;
                lightData.spotOuterAngle = light.spotOuterAngle;

                lightsData.push_back(lightData);
            }
        });

    memcpy(graphicsState.transformBufferMapped[graphicsState.currentFrame], graphicsState.modelMatrices, sizeof(glm::mat4) * MAX_INSTANCES);
    
    uint32_t lightCount = lightsData.size();
    // Copy light count to buffer (first uint)
    memcpy(graphicsState.lightsBufferMapped[graphicsState.currentFrame], &lightCount, sizeof(uint32_t));
    // Copy all light data to buffer (skip four uints for alignment purposes)
    if(!lightsData.empty())
        memcpy(&((uint32_t*)graphicsState.lightsBufferMapped[graphicsState.currentFrame])[4], lightsData.data(), sizeof(LightData) * lightsData.size());

    if(!graphicsState.instanceData.empty())
    {
        /* Sort instances by mesh and update instances buffer */

	    std::stable_sort(graphicsState.instanceData.begin(), graphicsState.instanceData.end(),
            [](const InstanceData& a, const InstanceData& b) -> bool { return a.meshIndex < b.meshIndex; });

        memcpy(graphicsState.instancesBufferMapped[graphicsState.currentFrame], graphicsState.instanceData.data(), sizeof(InstanceData) * graphicsState.instanceData.size());

        /* Populate draw batches */
        
        DrawBatch drawBatch{};
        drawBatch.meshIndex = graphicsState.instanceData[0].meshIndex;
        drawBatch.instanceOffset = 0;
        drawBatch.instanceCount = 1;

        graphicsState.drawBatches.clear();
        for(uint32_t i = 1; i < graphicsState.instanceData.size(); i++)
        {
            if(graphicsState.instanceData[i].meshIndex != drawBatch.meshIndex)
            {
                graphicsState.drawBatches.push_back(drawBatch);

                // Reset the draw batch
                drawBatch.meshIndex = graphicsState.instanceData[i].meshIndex;
                drawBatch.instanceOffset = i;
                drawBatch.instanceCount = 1;
            }
            else
                drawBatch.instanceCount++;
        }
        graphicsState.drawBatches.push_back(drawBatch);

        for(const DrawBatch& batch : graphicsState.drawBatches)
        {
            const Mesh& mesh = assets.GetMesh(batch.meshIndex);

            VkBuffer vertexBuffers[] = { mesh.vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), batch.instanceCount, 0, 0, batch.instanceOffset);
        }
    }

    /* --- Draw wireframes --- */

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsState.debugGraphicsPipeline);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphicsState.debugPipelineLayout,
		0,
		1,
		&graphicsState.descriptorSets[graphicsState.currentFrame],
		0,
		nullptr);

	uint32_t debugInstancesOffset = graphicsState.instanceData.size(); // Offset for the wireframe instances

    /* Gather wireframe instances from Debug Manager */
    graphicsState.instanceData.clear();
    for(auto& e : DebugManager::GetAllDebugEntities())
    {
        InstanceData instance{};
		instance.rendererIndex = debugInstancesOffset + graphicsState.instanceData.size();
        instance.meshIndex = e.first.mesh->index;
        instance.materialIndex = DebugManager::GetDebugMaterial()->index;

        graphicsState.instanceData.push_back(instance);

        graphicsState.modelMatrices[instance.rendererIndex] = e.first.transformMatrix;
    }

    memcpy(graphicsState.transformBufferMapped[graphicsState.currentFrame], graphicsState.modelMatrices, sizeof(glm::mat4) * MAX_INSTANCES);

    if(!graphicsState.instanceData.empty())
    {
        /* Sort instances by mesh and update instances buffer */

	    std::stable_sort(graphicsState.instanceData.begin(), graphicsState.instanceData.end(),
            [](const InstanceData& a, const InstanceData& b) -> bool { return a.meshIndex < b.meshIndex; });

		// Write the second half of the instances buffer with debug data
        memcpy(&((InstanceData*) graphicsState.instancesBufferMapped[graphicsState.currentFrame])[debugInstancesOffset], graphicsState.instanceData.data(), sizeof(InstanceData)* graphicsState.instanceData.size());

        /* Populate draw batches */
        
        DrawBatch drawBatch{};
        drawBatch.meshIndex = graphicsState.instanceData[0].meshIndex;
		drawBatch.instanceOffset = debugInstancesOffset; // Start at the first instance after the instance data from the last step
        drawBatch.instanceCount = 1;

        graphicsState.drawBatches.clear();
        for(uint32_t i = 1; i < graphicsState.instanceData.size(); i++)
        {
            if(graphicsState.instanceData[i].meshIndex != drawBatch.meshIndex)
            {
                graphicsState.drawBatches.push_back(drawBatch);

                // Reset the draw batch
                drawBatch.meshIndex = graphicsState.instanceData[i].meshIndex;
                drawBatch.instanceOffset = debugInstancesOffset + i;
                drawBatch.instanceCount = 1;
            }
            else
                drawBatch.instanceCount++;
        }
        graphicsState.drawBatches.push_back(drawBatch);

	    for(const DrawBatch& batch : graphicsState.drawBatches)
	    {
		    const Mesh& mesh = assets.GetMesh(batch.meshIndex);

		    VkBuffer vertexBuffers[] = { mesh.vertexBuffer };
		    VkDeviceSize offsets[] = { 0 };
		    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		    vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), batch.instanceCount, 0, 0, batch.instanceOffset);
	    }
    }

    DebugManager::UpdateAllWireframes(time.deltaTime);

    /* --- Draw skybox --- */

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsState.skyboxGraphicsPipeline);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphicsState.skyboxPipelineLayout,
		0,
		1,
		&graphicsState.descriptorSets[graphicsState.currentFrame],
		0,
		nullptr);

	const Mesh& mesh = assets.GetMesh(L"primitives\\cube.obj");

	// Bind the skybox vertex buffer
	VkBuffer vertexBuffers[] = { mesh.vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	// Draw the skybox
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);

#ifdef _DEBUG
	/* --- Draw ImGui UI --- */

    // Only draw immediate-mode GUI in debug builds
    ImGui_ImplVulkan_NewFrame();
    if(!window.isSurfaceOverridden)
        ImGui_ImplGlfw_NewFrame();
    else
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float) window.swapChainExtent.width, (float) window.swapChainExtent.height);
    }
    ImGui::NewFrame();

    // Create a full-screen, borderless window for the dockspace
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* imguiViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(imguiViewport->WorkPos);
    ImGui::SetNextWindowSize(imguiViewport->WorkSize);
    ImGui::SetNextWindowViewport(imguiViewport->ID);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    // Dockspace ID and creation
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    /* Draw UI */

    ImGui::Begin("FPS", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoMove);
    ImGui::Text("FPS: %.0f \t Average: %.0f \t Low: %.0f", time.GetFPS(), time.GetAverageFPS(), time.minFPS);
    ImGui::End();

    ImGui::Begin("Hello Vulkan");
    ImGui::Text("This is a Vulkan + ImGui integration!");
    ImGui::End();

    ImGui::Begin("Entities");
    if(ImGui::TreeNode("Entities"))
    {
        ctx.ForEachEntityInLevel([this, &ctx, &assets](const Entity& entity)
            {
                if(ctx.HasComponent<TransformComponent>(entity))
                    if(ImGui::TreeNode(std::format("Entity {}", entity).c_str()))
                    {
                        TransformComponent& transform = ctx.GetComponent<TransformComponent>(entity);

                        if(ImGui::TreeNode("Transform"))
                        {
                            glm::vec3 location = transform.location;
                            glm::vec3 eulerAngles = glm::eulerAngles(transform.rotation);
                            glm::vec3 scale = transform.scale;
                            ImGui::DragFloat3("Location", &location.x, 0.1f, -5.0f, 5.0f);
                            ImGui::DragFloat3("Rotation", &eulerAngles.x, 0.1f, -5.0f, 5.0f);
                            ImGui::DragFloat3("Scale", &scale.x, 0.1f, -5.0f, 5.0f);

                            if(location != transform.location)
                                transform.location = location;
                            if(eulerAngles != glm::eulerAngles(transform.rotation))
                            {
                                glm::vec3 eulerDeltas = eulerAngles - glm::eulerAngles(transform.rotation);

                                transform.rotation =
                                    glm::angleAxis(eulerDeltas.x, glm::vec3(1, 0, 0))
                                    * glm::angleAxis(eulerDeltas.y, glm::vec3(0, 1, 0))
                                    * glm::angleAxis(eulerDeltas.z, glm::vec3(0, 0, 1)) * transform.rotation;
                            }
                            if(scale != transform.scale)
                                transform.scale = scale;

                            ImGui::TreePop();
                        }

                        if(ctx.HasComponent<MeshRendererComponent>(entity))
                        {
                            MeshRendererComponent& meshRenderer = ctx.GetComponent<MeshRendererComponent>(entity);

                            if(ImGui::TreeNode("MeshRenderer"))
                            {
                                ImGui::Text("Renderer Index: %i", meshRenderer.rendererIndex);

                                int meshIndex = assets.GetMesh(meshRenderer.mesh.identifier).index;
                                int materialIndex = assets.GetMaterial(meshRenderer.material.identifier).index;
                                ImGui::DragInt("Mesh Index", &meshIndex, 0.1f, 0, assets.GetMeshes().size() - 1);
                                ImGui::DragInt("Material Index", &materialIndex, 0.1f, 0, assets.GetMaterials().size() - 1);

                                if(meshIndex != assets.GetMesh(meshRenderer.mesh.identifier).index)
                                    meshRenderer.mesh = MeshRef(assets.GetMesh(meshIndex).identifier);
                                if(materialIndex != assets.GetMaterial(meshRenderer.material.identifier).index)
                                    meshRenderer.material = MaterialRef(assets.GetMaterial(materialIndex).identifier);

                                ImGui::TreePop();
                            }
                        }

                        ImGui::TreePop();
                    }
            });

        ImGui::TreePop();
    }
    ImGui::End();

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);

    if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
#endif

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer!");
}

void GraphicsSystem::UpdateUniformBuffer(const GraphicsState& graphicsState, const Cameras& cameras, SystemContext<const TransformComponent, const CameraComponent> ctx, uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.view = cameras.GetMainViewMatrix(ctx.GetContext<const TransformComponent>());
    ubo.proj = cameras.GetMainProjectionMatrix(ctx.GetContext<const CameraComponent>());
    ubo.proj[1][1] *= -1;

    memcpy(graphicsState.cameraUBOsMapped[currentImage], &ubo, sizeof(ubo));
}