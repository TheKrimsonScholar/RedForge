#pragma once

#include "EditorPanel.h"

#include "DebugManager.h"

#include <QTextBrowser>
#include <QLineEdit>
#include <QLabel>

class ConsolePanel : public EditorPanel
{
	Q_OBJECT

private:
	std::streambuf* coutBuffer = nullptr;
	std::stringstream consoleStream;

	QTextBrowser* text;
	std::vector<QLabel*> lines;

public:
	ConsolePanel(QWidget* parent = nullptr);
	~ConsolePanel();

protected:
	void Initialize() override;
	void Update() override;

	void OnLogMessagePrinted(const LogMessage& message);

	friend class MainEditorWindow;
};