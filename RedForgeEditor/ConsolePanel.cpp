#include "ConsolePanel.h"

#include <iostream>

#include "DebugMacros.h"
#include "DebugManager.h"

#include <QLayout>
#include <QTimer>

ConsolePanel::ConsolePanel(QWidget* parent) : EditorPanel("Console", parent), 
	consoleStream()
{
	text = new QTextBrowser(this);

	QVBoxLayout* vBox = new QVBoxLayout();
	vBox->setAlignment(Qt::AlignTop);
	vBox->addWidget(text);

	contentArea->setLayout(vBox);
}
ConsolePanel::~ConsolePanel()
{
	
}

void ConsolePanel::Initialize()
{
	for(const DebugLogEvent& message : DebugManager::GetDebugLog())
		text->append(message.ToString().c_str());

	DebugManager::GetOnLogMessagePrinted()->AddUnique(EventCallback(this, &ConsolePanel::OnLogMessagePrinted));
}
void ConsolePanel::Update()
{

}

void ConsolePanel::OnLogMessagePrinted(const DebugLogEvent& message)
{
	text->append(message.ToString().c_str());
}