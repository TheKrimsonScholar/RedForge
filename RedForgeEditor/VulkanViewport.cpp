#include "VulkanViewport.h"

#include "Engine.h"

#define IMGUI_ENABLE_VIEWPORTS
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_vulkan.h"

#include <QResizeEvent>
#include <QTimer>

VulkanViewport::VulkanViewport(QWidget* parent)
	: engine(nullptr), vulkanInstance(new QVulkanInstance()), camera(new ViewportCamera())
{
	vulkanInstance->setVkInstance(GraphicsSystem::GetVulkanInstance());
	vulkanInstance->create();

	setSurfaceType(QSurface::VulkanSurface);
	setVulkanInstance(vulkanInstance);
}
VulkanViewport::~VulkanViewport()
{
	delete camera;
	delete vulkanInstance;
}

void VulkanViewport::Initialize()
{

}

void VulkanViewport::Update()
{
	camera->Update();
}

bool VulkanViewport::event(QEvent* event)
{
	switch(event->type())
	{
		case QEvent::PlatformSurface:
		{
			QPlatformSurfaceEvent* surfaceEvent = static_cast<QPlatformSurfaceEvent*>(event);
			if(surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated)
				OnSurfaceCreated();
			else if(surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
				OnSurfaceDestroyed();
		}
		break;
		default: break;
	}

	return QWindow::event(event);
}

void VulkanViewport::OnSurfaceCreated()
{
	// Use QTimer::singleShot(0, ...) to ensure the initialization runs after all current event processing
	QTimer::singleShot(0, this, [this]()
		{
			VkSurfaceKHR surface = vulkanInstance->surfaceForWindow(this);
			if(surface != VK_NULL_HANDLE)
			{
				onSurfaceInitialized(surface);
			}
		});
}
void VulkanViewport::OnSurfaceDestroyed()
{

}