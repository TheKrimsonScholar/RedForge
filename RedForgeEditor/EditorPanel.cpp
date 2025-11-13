#include "EditorPanel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>

#include "EditorPaths.h"

EditorPanel::EditorPanel(const QString& title, QWidget* parent) : QDockWidget(title, parent)
{
	setFocusPolicy(Qt::FocusPolicy::ClickFocus);

	/* Custom titlebar widget */

	titlebar = new QWidget(this);

	QLabel* titleLabel = new QLabel(title, this);
	titleLabel->setObjectName("title");
	
	QToolButton* floatButton = new QToolButton(this);
	floatButton->setIcon(QICON_FROM_PATH("Basic/Float"));
	floatButton->setIconSize(QSize(12, 12));
	QObject::connect(this, &EditorPanel::dockLocationChanged, this,
		[this, floatButton](Qt::DockWidgetArea area)
		{
			floatButton->setIcon(isFloating() ? QICON_FROM_PATH("Basic/Dock") : QICON_FROM_PATH("Basic/Float"));
		});
	QObject::connect(floatButton, &QToolButton::clicked, this, 
		[this, floatButton]()
		{
			setFloating(!isFloating());
		});
	floatButton->setObjectName("float-button");

	QToolButton* closeButton = new QToolButton(this);
	closeButton->setIcon(QICON_FROM_PATH("Basic/Cross"));
	closeButton->setIconSize(QSize(12, 12));
	QObject::connect(closeButton, &QToolButton::clicked, this, &EditorPanel::close);
	closeButton->setObjectName("close-button");

	QHBoxLayout* hBox = new QHBoxLayout();
	hBox->addWidget(titleLabel);
	hBox->addWidget(floatButton);
	hBox->addWidget(closeButton);
	titlebar->setLayout(hBox);

	setTitleBarWidget(titlebar);

	/* Scrollable content area */

	scrollArea = new QScrollArea(this);
	setWidget(scrollArea);

	contentArea = new QWidget(this);
	
	scrollArea->setWidget(contentArea);
	scrollArea->setWidgetResizable(true);
}
EditorPanel::~EditorPanel()
{
	
}