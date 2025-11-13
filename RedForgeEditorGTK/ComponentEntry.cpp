//#include "ComponentEntry.h"
//
//#include "Material.h"
//#include "EntityManager.h"
//
//#include <gtkmm/spinbutton.h>
//#include <glibmm.h>
//
//#include "ComponentVariableEntry_Float.h"
//#include "ComponentVariableEntry_Vector2.h"
//#include "ComponentVariableEntry_Vector3.h"
//#include "ComponentVariableEntry_QuaternionEuler.h"
//#include "ComponentVariableEntry_Mesh.h"
//#include "ComponentVariableEntry_Material.h"
//
//ComponentEntry::ComponentEntry(Entity entity, std::type_index componentTypeID, void* componentPtr) : Gtk::Expander(GET_COMPONENT_NAME(componentTypeID)), 
//	entity(entity), componentTypeID(componentTypeID), variablesBox(Gtk::Orientation::VERTICAL)
//{
//	set_child(variablesBox);
//
//	std::vector<std::pair<void*, ComponentVariableInfo>> variables = GET_COMPONENT_VARS(componentTypeID, componentPtr);
//    for(auto& member : variables)
//		AppendVariableWidget(member.second, member.first);
//
//	timeoutConnection = Glib::signal_timeout().connect([this]() -> bool
//		{
//			if(EntityManager::IsComponentValid(this->entity, this->componentTypeID))
//				UpdateDisplayedVariables();
//
//			return true;
//		}, 100);
//}
//ComponentEntry::~ComponentEntry()
//{
//	if(timeoutConnection.connected())
//		timeoutConnection.disconnect();
//
//	for(ComponentVariableEntry* variableEntry : variableEntries)
//		delete variableEntry;
//	variableEntries.clear();
//}
//
//void ComponentEntry::AppendVariableWidget(ComponentVariableInfo variableInfo, void* variablePtr)
//{
//	ComponentVariableEntry* varEntry = nullptr;
//	if(variableInfo.variableType == typeid(float)) varEntry = new ComponentVariableEntry_Float(variableInfo.variableName, variablePtr);
//	if(variableInfo.variableType == typeid(glm::vec2)) varEntry = new ComponentVariableEntry_Vector2(variableInfo.variableName, variablePtr);
//	if(variableInfo.variableType == typeid(glm::vec3)) varEntry = new ComponentVariableEntry_Vector3(variableInfo.variableName, variablePtr);
//	if(variableInfo.variableType == typeid(glm::quat)) varEntry = new ComponentVariableEntry_QuaternionEuler(variableInfo.variableName, variablePtr);
//	if(variableInfo.variableType == typeid(MeshRef)) varEntry = new ComponentVariableEntry_Mesh(variableInfo.variableName, variablePtr);
//	if(variableInfo.variableType == typeid(MaterialRef)) varEntry = new ComponentVariableEntry_Material(variableInfo.variableName, variablePtr);
//	
//	assert(varEntry && "Serialized component variable's type is not registered!");
//
//	variableEntries.push_back(varEntry);
//	variablesBox.append(*varEntry);
//}
//
//void ComponentEntry::UpdateDisplayedVariables()
//{
//	for(ComponentVariableEntry* varEntry : variableEntries)
//		varEntry->UpdateDisplayedValue();
//}