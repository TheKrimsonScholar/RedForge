#include "MainEditorWindow.h"

#include "Editor.h"

#include "Engine.h"
#include "DebugCameraState.h"

#include <QTimer>

#include "HierarchyPanel.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

MainEditorWindow::MainEditorWindow(QWidget* parent) : QMainWindow(parent)
{
	Instance = this;

	Engine* engine = Editor::CreateEngineInstance();

	viewport = new VulkanViewport(engine);
	viewport->onSurfaceInitialized = 
		[this, engine](VkSurfaceKHR surface)
		{
			EngineStartupParams startupParams = engine->CreateStartupParams();
			startupParams.surfaceOverride = surface;

			engine->GetWorld().GetResource<InputState>().SetActiveInputLayer(startupParams, inputLayer);
			engine->GetWorld().GetResource<DebugCameraState>().isActive = true;

			engine->Startup(startupParams);
			Initialize();

			QTimer* updateTimer = new QTimer(this);
			QObject::connect(updateTimer, &QTimer::timeout, this,
				[this, engine]()
				{
					engine->Update(frameTimer.restart() / 1000.0f);
					Update();
				});
			frameTimer.start();
			updateTimer->start(0);
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
	Editor::DestroyEngineInstance();

	delete inputLayer;
}

void MainEditorWindow::Initialize()
{
	Editor::GetEntityManager().LoadLevel(L"Levels/Level.txt");

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
	EngineShutdownParams shutdownParams = Editor::GetEngine()->CreateShutdownParams();
	shutdownParams.shouldDestroyVulkanInstance = false;
	
	Editor::GetEngine()->Shutdown(shutdownParams);
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