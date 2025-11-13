//#pragma once
//
//#include "GTKInputLayer.h"
//
//#include <gtkmm.h>
//
//#include <epoxy/gl.h>
//
//#ifdef _WIN32
//    #define VK_USE_PLATFORM_WIN32_KHR
//    #define WIN32_LEAN_AND_MEAN
//    #define NOMINMAX
//    #include <windows.h>
//    #include <windowsx.h>
//#else
//    #define VK_USE_PLATFORM_XLIB_KHR
//#endif
//#include <vulkan/vulkan.h>
//#include "Engine.h"
//#include "ViewportCamera.h"
//
//class VulkanViewport : public Gtk::GLArea
//{
//public:
//    Engine* engine;
//    ViewportCamera* camera;
//
//    std::function<void()> onViewportInitialized;
//    std::function<void()> onViewportResized;
//    std::function<void()> onViewportUpdated;
//
//private:
//    GTKInputLayer inputLayer;
//    Glib::RefPtr<Gtk::GestureClick> clickGesture;
//    Glib::RefPtr<Gtk::GestureDrag> dragGesture;
//    Glib::RefPtr<Gtk::EventControllerKey> keyController;
//
//    int viewportWidth = 0, viewportHeight = 0;
//
//    GLuint sharedTexture = 0;
//    GLuint memoryObject = 0;
//
//    GLuint waitSemaphore;
//    GLuint signalSemaphore;
//
//    GLuint shaderProgram = 0;
//    GLuint quadVAO = 0;
//    GLuint quadVBO = 0;
//
//public:
//    VulkanViewport();
//    ~VulkanViewport();
//
//protected:
//    void on_realize() override;
//    void on_unrealize() override;
//
//    void on_resize(int width, int height) override;
//
//    bool on_drop(const Glib::ValueBase& value, double x, double y);
//
//    void CreateSharedResources();
//    void SetupOpenGLInterop();
//    void ImportVulkanMemory();
//    void ImportVulkanSemaphores();
//    void SetupOpenGLRendering();
//    GLuint CreateShaderProgram(const char* vertexSource, const char* fragmentSource);
//
//    void DestroySharedResources();
//
//    bool on_render(const Glib::RefPtr<Gdk::GLContext>& context) override;
//
//    void CheckOpenGLExtensions();
//};