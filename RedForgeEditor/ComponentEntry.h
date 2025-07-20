#pragma once

#include <typeindex>

#include "ComponentMacros.h"
#include "EntityManager.h"

#include "gtkmm/expander.h"
#include "gtkmm/label.h"
#include "gtkmm/box.h"

#include "ComponentVariableEntry.h"

class ComponentEntry : public Gtk::Expander
{
private:
	Entity entity;
	std::type_index componentTypeID;

	Gtk::Box variablesBox;

	std::vector<ComponentVariableEntry*> variableEntries;

	sigc::connection timeoutConnection;

public:
	ComponentEntry(Entity entity, std::type_index componentTypeID, void* componentPtr);
	~ComponentEntry();

	//void PopulateComponentData(std::type_index componentTypeID, void* componentPtr);
	void AppendVariableWidget(ComponentVariableInfo variableInfo, void* variablePtr);

	void UpdateDisplayedVariables();
};