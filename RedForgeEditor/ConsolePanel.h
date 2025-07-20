#pragma once

#include "EditorPanel.h"

#include "gtkmm/textview.h"

class ConsolePanel : public EditorPanel
{
private:
	std::streambuf* coutBuffer = nullptr;
	std::stringstream consoleStream;

	Gtk::TextView text;

public:
	ConsolePanel();
	~ConsolePanel();
};