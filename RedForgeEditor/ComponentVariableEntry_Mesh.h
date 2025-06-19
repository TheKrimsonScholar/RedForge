#pragma once

#include "ComponentVariableEntry.h"

#include "Mesh.h"

#include <gtkmm/dropdown.h>

class ComponentVariableEntry_Mesh : public ComponentVariableEntry
{
private:
	Mesh** variablePtr;

	Gtk::DropDown dropdown;

	std::vector<Mesh*> meshes;

public:
	ComponentVariableEntry_Mesh(const std::string& label, void* variablePtr);
	~ComponentVariableEntry_Mesh();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};