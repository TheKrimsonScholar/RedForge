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
		case QEvent::Drop:
		{
			QDropEvent* dropEvent = static_cast<QDropEvent*>(event);
			const QMimeData* mimeData = dropEvent->mimeData();
			
			// Only accept file drops (for instantiating assets in level)
			if(mimeData->hasFormat("application/x-filelistitem"))
			{
			    QByteArray encodedData = mimeData->data("application/x-filelistitem");
			    QDataStream stream(&encodedData, QIODevice::ReadOnly);
			    QString itemData;
			
				std::vector<std::filesystem::path> filesToDrop;
			    while(!stream.atEnd())
			    {
					int r, c;
					QMap<int, QVariant> roleDataMap;
					stream >> r >> c >> roleDataMap;

					filesToDrop.emplace_back(roleDataMap[Qt::UserRole].value<QString>().toStdString());
			    }

				for(const std::filesystem::path& file : filesToDrop)
					OnFileDroppedIntoViewport(file);

				dropEvent->acceptProposedAction();
			}
			else
				dropEvent->ignore();
		}
		break;
		case QEvent::DragEnter:
		{
			QDragEnterEvent* dragEnterEvent = static_cast<QDragEnterEvent*>(event);
			if(dragEnterEvent->mimeData()->hasFormat("application/x-filelistitem"))
				dragEnterEvent->acceptProposedAction();
			else
				dragEnterEvent->ignore();
		}
		break;
		case QEvent::DragMove:
		{
			QDragMoveEvent* dragMoveEvent = static_cast<QDragMoveEvent*>(event);
			if(dragMoveEvent->mimeData()->hasFormat("application/x-filelistitem"))
				dragMoveEvent->acceptProposedAction();
			else
				dragMoveEvent->ignore();
		}
		break;
		default: break;
	}

	return QWindow::event(event);
}

void VulkanViewport::OnFileDroppedIntoViewport(const std::filesystem::path& filePath)
{
	if(filePath.extension() == ".entity")
	{
		// Spawn the entity under the level root
		LevelManager::LoadEntityFromPrefab(filePath, {});
	}
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