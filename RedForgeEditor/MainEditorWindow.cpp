#include "MainEditorWindow.h"

#include "Engine.h"

#include <QTimer>

#include "HierarchyPanel.h"

MainEditorWindow::MainEditorWindow(QWidget* parent) : QMainWindow(parent)
{
	Instance = this;

	engine = new Engine();
	engine->CreateVulkanInstance();

	viewport = new VulkanViewport();
	viewport->SetEngine(engine);
	viewport->onSurfaceInitialized = 
		[this](VkSurfaceKHR surface)
		{
			engine->Startup(surface);
			Initialize();

			QTimer* updateTimer = new QTimer(this);
			QObject::connect(updateTimer, &QTimer::timeout, this,
				[this]()
				{
					engine->Update();
					Update();
				});
			updateTimer->start(1000 / 60);
		};

	inputLayer = new QtInputLayer(viewport);
	
	toolbar = new EditorToolbar(this);

	QWidget* vulkanWidgetWrapper = QWidget::createWindowContainer(viewport, this);

	QWidget* centralWidget = new QWidget(this);
	QVBoxLayout* vBox = new QVBoxLayout();
	vBox->addWidget(toolbar);
	vBox->addWidget(vulkanWidgetWrapper);
	centralWidget->setLayout(vBox);

	setCentralWidget(centralWidget);

	hierarchyPanel = new HierarchyPanel(this);
	addDockWidget(Qt::RightDockWidgetArea, hierarchyPanel);

	fileBrowserPanel = new FileBrowserPanel(this);
	addDockWidget(Qt::LeftDockWidgetArea, fileBrowserPanel);

	inspectorPanel = new InspectorPanel(this);
	addDockWidget(Qt::RightDockWidgetArea, inspectorPanel);

	consolePanel = new ConsolePanel(this);
	addDockWidget(Qt::BottomDockWidgetArea, consolePanel);

	//vulkanWidgetWrapper->setMinimumSize(640, 480);
}
MainEditorWindow::~MainEditorWindow()
{
	engine->DestroyVulkanInstance();

	delete inputLayer;
	delete engine;
}

void MainEditorWindow::Initialize()
{
	InputSystem::SetActiveInputLayer(inputLayer);
	
	CameraManager::SetViewMatrixOverride(&viewport->GetCamera()->viewMatrix);
	CameraManager::SetProjectionMatrixOverride(&viewport->GetCamera()->projectionMatrix);

	LevelManager::LoadLevel(L"Levels/Level.txt");

	viewport->Initialize();
	hierarchyPanel->Initialize();
	fileBrowserPanel->Initialize();
	inspectorPanel->Initialize();
	consolePanel->Initialize();
}

void MainEditorWindow::Update()
{
	viewport->Update();
	hierarchyPanel->Update();
	fileBrowserPanel->Update();
	inspectorPanel->Update();
	consolePanel->Update();
}

void MainEditorWindow::changeEvent(QEvent* event)
{
	switch(event->type())
	{
		case QEvent::WindowStateChange:
			{
				if(isMinimized())
					OnWindowMinimized();
				else if(isMaximized())
					OnWindowMaximized();
				else
					OnWindowRestored();
			}
			break;
	}

	QMainWindow::changeEvent(event);
}

void MainEditorWindow::closeEvent(QCloseEvent* event)
{
	engine->Shutdown(false);
}

void MainEditorWindow::OnWindowMinimized()
{
	qDebug() << "Window was minimized!";
}
void MainEditorWindow::OnWindowRestored()
{
	qDebug() << "Window was restored!";
}
void MainEditorWindow::OnWindowMaximized()
{
	qDebug() << "Window was maximized!";
}