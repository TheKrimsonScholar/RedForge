//#pragma once
//
//#include "ComponentVariableEntry.h"
//
//#include "MeshRef.h"
//
//#include <gtkmm/dropdown.h>
//
//class ComponentVariableEntry_Mesh : public ComponentVariableEntry
//{
//private:
//	MeshRef* variablePtr;
//
//	Gtk::DropDown dropdown;
//
//	std::vector<MeshRef> meshes;
//
//public:
//	ComponentVariableEntry_Mesh(const std::string& label, void* variablePtr);
//	~ComponentVariableEntry_Mesh();
//
//private:
//	virtual void UpdateDisplayedValue() override;
//	virtual void OnValueChanged() override;
//};