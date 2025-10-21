#include "VulkanViewport.h"

#include <iostream>

#include <gdk/gdk.h>
#include <gdk/gdkglcontext.h>

#include <gdkmm/texture.h>
#include <glibmm/bytes.h>

VulkanViewport::VulkanViewport() : 
    camera(new ViewportCamera()), inputLayer(this)
{
    set_focusable(true);
    set_auto_render(true);

    add_tick_callback([this](const Glib::RefPtr<Gdk::FrameClock>& frame_clock) -> bool
        {
            queue_render();
            return true;
        });

    // Accept string data drops for assets from file browser
    auto drop_target = Gtk::DropTarget::create(Glib::Value<Glib::ustring>::value_type(), Gdk::DragAction::COPY);
    drop_target->signal_drop().connect(sigc::mem_fun(*this, &VulkanViewport::on_drop), false);
    add_controller(drop_target);
}
VulkanViewport::~VulkanViewport()
{

}

void VulkanViewport::on_realize()
{
    Gtk::GLArea::on_realize();
    make_current();

    CheckOpenGLExtensions();

    // Get initial size
    viewportWidth = get_allocated_width();
    viewportHeight = get_allocated_height();

    CreateSharedResources();
}
void VulkanViewport::on_unrealize()
{
    DestroySharedResources();
    Gtk::GLArea::on_unrealize();
}

void VulkanViewport::on_resize(int width, int height)
{
    if(width != viewportWidth || height != viewportHeight)
    {
        viewportWidth = width;
        viewportHeight = height;

        GraphicsSystem::SetExternalRenderImageExtent(width, height);

        // Recreate resources with new size
        GraphicsSystem::RecreateSwapChain();
        DestroySharedResources();
        CreateSharedResources();
    }

    Gtk::GLArea::on_resize(width, height);
}

bool VulkanViewport::on_drop(const Glib::ValueBase& value, double x, double y)
{
    Glib::ustring text;

    Glib::Value<Glib::ustring> asset;
    asset.init(value.gobj());
    
    text = asset.get();

    std::cout << "DROP ON VIEWPORT" << std::endl;
    
    if (!text.empty())
    {
        // Custom drop logic
        g_print("Dropped item onto viewport: %s\n", text.c_str());
    }

    return true; // drop handled
}

void VulkanViewport::CreateSharedResources()
{
    if(viewportWidth <= 0 || viewportHeight <= 0)
        return;

	static bool initialized = false;

    if(!initialized)
    {
        initialized = true;

        onViewportInitialized();
        InputSystem::SetActiveInputLayer(&inputLayer);
    }
    else
        onViewportResized();

    // Set up the OpenGL side after Vulkan is initialized
    SetupOpenGLInterop();
}
void VulkanViewport::SetupOpenGLInterop()
{
    // Import the resources created by Vulkan
    ImportVulkanMemory();
    ImportVulkanSemaphores();

    // Setup resources for rendering the texture
    SetupOpenGLRendering();
}
void VulkanViewport::ImportVulkanMemory()
{
    size_t memorySize = GraphicsSystem::GetExternalRenderMemorySize();
    #ifdef _WIN32
        // Get the exported memory handle from Vulkan engine
        void* memoryHandle = GraphicsSystem::GetExternalRenderMemoryHandle();

        // Create OpenGL memory object
        glCreateMemoryObjectsEXT(1, &memoryObject);
        glImportMemoryWin32HandleEXT(memoryObject, memorySize, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, memoryHandle);
    #else
        // Get the exported memory handle from Vulkan engine
        int memoryFd = GraphicsSystem::GetExternalRenderMemoryHandle();

        // Create OpenGL memory object
        glCreateMemoryObjectsEXT(1, &memoryObject);
        glImportMemoryFdEXT(memoryObject, memorySize, GL_HANDLE_TYPE_OPAQUE_FD_EXT, memoryFd);
    #endif

    // Create OpenGL texture from imported memory
    glCreateTextures(GL_TEXTURE_2D, 1, &sharedTexture);
    glTextureStorageMem2DEXT(sharedTexture, 1, GL_RGBA8,
        viewportWidth, viewportHeight,
        memoryObject, 0);

    // Set texture parameters
    glTextureParameteri(sharedTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(sharedTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(sharedTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(sharedTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
void VulkanViewport::ImportVulkanSemaphores()
{
    #ifdef _WIN32
        // Get exported semaphore handles from Vulkan
        void* renderCompleteHandle = GraphicsSystem::GetExternalRenderCompleteSemaphoreHandle();
        void* renderReleaseHandle = GraphicsSystem::GetExternalRenderReleaseSemaphoreHandle();

        // Import semaphores into OpenGL
        glGenSemaphoresEXT(1, &waitSemaphore);
        glGenSemaphoresEXT(1, &signalSemaphore);

        glImportSemaphoreWin32HandleEXT(waitSemaphore, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, renderCompleteHandle);
        glImportSemaphoreWin32HandleEXT(signalSemaphore, GL_HANDLE_TYPE_OPAQUE_WIN32_EXT, renderReleaseHandle);
    #else
        // Get exported semaphore handles from Vulkan
        int renderCompleteFd = GraphicsSystem::GetExternalRenderCompleteSemaphoreHandle();
        int renderReleaseFd = GraphicsSystem::GetExternalRenderReleaseSemaphoreHandle();

        // Import semaphores into OpenGL
        glGenSemaphoresEXT(1, &waitSemaphore);
        glGenSemaphoresEXT(1, &signalSemaphore);

        glImportSemaphoreFdEXT(waitSemaphore, GL_HANDLE_TYPE_OPAQUE_FD_EXT, renderCompleteFd);
        glImportSemaphoreFdEXT(signalSemaphore, GL_HANDLE_TYPE_OPAQUE_FD_EXT, renderReleaseFd);
    #endif
}
void VulkanViewport::SetupOpenGLRendering()
{
    // Create shader program for displaying the texture
    static const char* VERTEX_SHADER_SOURCE = R"(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            layout (location = 1) in vec2 aTexCoord;
            
            out vec2 TexCoord;
            
            void main()
            {
                gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
                TexCoord = aTexCoord;
            }
        )";

    static const char* FRAGMENT_SHADER_SOURCE = R"(
            #version 330 core
            out vec4 FragColor;
            
            in vec2 TexCoord;
            uniform sampler2D ourTexture;
            
            void main()
            {
                FragColor = texture(ourTexture, TexCoord);
            }
        )";

    shaderProgram = CreateShaderProgram(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);

    // Create fullscreen quad
    static const float QUAD_VERTICES[] = 
    {
        // positions   // texture coords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), QUAD_VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
GLuint VulkanViewport::CreateShaderProgram(const char* vertex_source, const char* fragment_source)
{
    // Standard OpenGL shader compilation code
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

void VulkanViewport::DestroySharedResources()
{
    
}

bool VulkanViewport::on_render(const Glib::RefPtr<Gdk::GLContext>& context)
{
    // The engine is updated in the callback
    onViewportUpdated();

    camera->Update();

    // Wait for Vulkan to complete (synchronization)
    if(waitSemaphore)
        glWaitSemaphoreEXT(waitSemaphore, 0, nullptr, 0, nullptr, nullptr);

    // Display the shared texture
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, sharedTexture);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Signal OpenGL is done
    if(signalSemaphore)
        glSignalSemaphoreEXT(signalSemaphore, 0, nullptr, 0, nullptr, nullptr);

    return true;
}

void VulkanViewport::CheckOpenGLExtensions()
{
    if(!epoxy_has_gl_extension("GL_EXT_memory_object"))
        throw std::runtime_error("GL_EXT_memory_object not supported!");
    if(!epoxy_has_gl_extension("GL_EXT_semaphore"))
        throw std::runtime_error("GL_EXT_semaphore not supported!");
    #ifdef _WIN32
        if(!epoxy_has_gl_extension("GL_EXT_memory_object_win32"))
            throw std::runtime_error("GL_EXT_memory_object_win32 not supported!");
        if(!epoxy_has_gl_extension("GL_EXT_semaphore_win32"))
            throw std::runtime_error("GL_EXT_semaphore_win32 not supported!");
    #endif
}