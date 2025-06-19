#pragma once

#include "EntityManager.h"

#include "gtkmm\scrolledwindow.h"
#include "gtkmm\box.h"
#include "gtkmm\label.h"
#include "gtkmm\listbox.h"
#include "gtkmm\adjustment.h"
#include "gtkmm\spinbutton.h"

#include "ComponentEntry.h"

class InspectorWindow : public Gtk::ScrolledWindow
{
private:
	Gtk::Box contentArea;

	Gtk::Label windowLabel;

	Gtk::Label entityLabel;
	Gtk::ListBox componentsList;

public:
	InspectorWindow();
	~InspectorWindow();

	void SetTarget(Entity entity);

	void ResetTarget();
};