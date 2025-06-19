#pragma once

#include <typeindex>

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/spinbutton.h>

class ComponentVariableEntry : public Gtk::Box
{
protected:
	Gtk::Label variableLabel;

public:
	ComponentVariableEntry(const std::string& label, void* variablePtr);
	~ComponentVariableEntry();

	virtual void UpdateDisplayedValue() = 0;
	virtual void OnValueChanged() = 0;
};