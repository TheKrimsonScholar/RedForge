//#include "ComponentVariableEntry_Mesh.h"
//
//#include "ResourceManager.h"
//#include "PathUtils.h"
//
//ComponentVariableEntry_Mesh::ComponentVariableEntry_Mesh(const std::string& label, void* variablePtr) : ComponentVariableEntry(label, variablePtr), 
//	variablePtr(static_cast<MeshRef*>(variablePtr))
//{
//	std::vector<Glib::ustring> meshNames;
//	for(auto& mesh : ResourceManager::GetMeshMap())
//	{
//		meshNames.push_back(Glib::ustring(WideToNarrow(mesh.first)));
//		meshes.emplace_back(mesh.first);
//	}
//	
//	dropdown = Gtk::DropDown(meshNames);
//
//	dropdown.signal_state_flags_changed().connect([this](Gtk::StateFlags flags)
//		{
//			*this->variablePtr = meshes[(uint32_t) dropdown.get_selected()];
//			//OnValueChanged();
//		});
//
//	append(dropdown);
//}
//ComponentVariableEntry_Mesh::~ComponentVariableEntry_Mesh()
//{
//
//}
//
//void ComponentVariableEntry_Mesh::UpdateDisplayedValue()
//{
//
//}
//void ComponentVariableEntry_Mesh::OnValueChanged()
//{
//
//}