//#pragma once
//
//#include "ComponentVariableEntry.h"
//
//#include "MaterialRef.h"
//
//#include <gtkmm/dropdown.h>
//
//class ComponentVariableEntry_Material : public ComponentVariableEntry
//{
//private:
//	MaterialRef* variablePtr;
//
//	Gtk::DropDown dropdown;
//
//	std::vector<MaterialRef> materials;
//
//public:
//	ComponentVariableEntry_Material(const std::string& label, void* variablePtr);
//	~ComponentVariableEntry_Material();
//
//private:
//	virtual void UpdateDisplayedValue() override;
//	virtual void OnValueChanged() override;
//};