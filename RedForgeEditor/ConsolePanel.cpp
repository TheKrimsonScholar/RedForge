#include "ConsolePanel.h"

#include <iostream>

ConsolePanel::ConsolePanel() : EditorPanel("Console"), 
	consoleStream(), text()
{
	text.add_css_class("console-text");

	contentArea.append(text);

	coutBuffer = std::cout.rdbuf();

	// Redirect cout to the custom stream
	std::cout.rdbuf(consoleStream.rdbuf());

	// Keep the console text up-to-date with the output stream
	add_tick_callback([this](const Glib::RefPtr<Gdk::FrameClock>& frame_clock) -> bool
		{
			Glib::ustring updatedText = consoleStream.str();
			Glib::RefPtr<Gtk::TextBuffer> textBuffer = text.get_buffer();
			if(updatedText == textBuffer->get_text())
				return true;

			textBuffer->set_text(updatedText);

			return true;
		});
}
ConsolePanel::~ConsolePanel()
{
	// Restore original cout buffer
	std::cout.rdbuf(coutBuffer);
}