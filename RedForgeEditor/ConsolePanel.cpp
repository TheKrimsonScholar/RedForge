#include "ConsolePanel.h"

#include <iostream>

#include "DebugMacros.h"

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
	for(const LogMessage& message : DebugManager::GetDebugLog())
		text->append(message.ToString().c_str());

	DebugManager::GetOnLogMessagePrinted()->AddUnique(EventCallback(this, &ConsolePanel::OnLogMessagePrinted));
}
void ConsolePanel::Update()
{

}

void ConsolePanel::OnLogMessagePrinted(const LogMessage& message)
{
	text->append(message.ToString().c_str());
}