#pragma once

#include "QtGui\QVulkanWindow.h"
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include "QtInputLayer.h"
#include "ViewportCamera.h"

class Engine;

class VulkanViewport : public QWindow
{
    Q_OBJECT

private:
    Engine* engine;
    QVulkanInstance* vulkanInstance;
    ViewportCamera* camera;

    std::function<void(VkSurfaceKHR)> onSurfaceInitialized;

public:
    VulkanViewport(QWidget* parent = nullptr);
    ~VulkanViewport();

    ViewportCamera* GetCamera() const { return camera; }

    void SetEngine(Engine* engine) { this->engine = engine; }

protected:
    void Initialize();
    void Update();

    bool event(QEvent* event) override;

    void OnFileDroppedIntoViewport(const std::filesystem::path& filePath);

    void OnSurfaceCreated();
    void OnSurfaceDestroyed();

    friend class MainEditorWindow;
};