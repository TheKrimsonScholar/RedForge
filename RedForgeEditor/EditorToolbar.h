#pragma once

#include "gtkmm/box.h"
#include "gtkmm/button.h"

class EditorToolbar : public Gtk::Box
{
private:
	Gtk::Button saveButton;

public:
	EditorToolbar();
	~EditorToolbar();

private:
	void SaveLevel();
};