#include "ComponentVariableEntry.h"

#include "EntityManager.h"

#include <glm/glm.hpp>

#include <gtkmm/spinbutton.h>
#include <glibmm.h>

ComponentVariableEntry::ComponentVariableEntry(const std::string& label, void* variablePtr) : Gtk::Box(Gtk::Orientation::HORIZONTAL),
	variableLabel(label)
{
	append(variableLabel);
}
ComponentVariableEntry::~ComponentVariableEntry()
{
	
}