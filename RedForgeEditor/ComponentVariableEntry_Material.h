#pragma once

#include "ComponentVariableEntry.h"

#include "Material.h"

#include <gtkmm/dropdown.h>

class ComponentVariableEntry_Material : public ComponentVariableEntry
{
private:
	Material** variablePtr;

	Gtk::DropDown dropdown;

	std::vector<Material*> materials;

public:
	ComponentVariableEntry_Material(const std::string& label, void* variablePtr);
	~ComponentVariableEntry_Material();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};