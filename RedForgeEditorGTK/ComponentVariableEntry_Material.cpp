//#include "ComponentVariableEntry_Material.h"
//
//#include "ResourceManager.h"
//#include "PathUtils.h"
//
//ComponentVariableEntry_Material::ComponentVariableEntry_Material(const std::string& label, void* variablePtr) : ComponentVariableEntry(label, variablePtr), 
//	variablePtr(static_cast<MaterialRef*>(variablePtr))
//{
//	std::vector<Glib::ustring> materialNames;
//	for(auto& material : ResourceManager::GetMaterialMap())
//	{
//		materialNames.push_back(Glib::ustring(WideToNarrow(material.first)));
//		materials.push_back(material.first);
//	}
//	
//	dropdown = Gtk::DropDown(materialNames);
//
//	dropdown.signal_state_flags_changed().connect([this](Gtk::StateFlags flags)
//		{
//			*this->variablePtr = materials[(uint32_t) dropdown.get_selected()];
//			//OnValueChanged();
//		});
//
//	append(dropdown);
//}
//ComponentVariableEntry_Material::~ComponentVariableEntry_Material()
//{
//
//}
//
//void ComponentVariableEntry_Material::UpdateDisplayedValue()
//{
//
//}
//void ComponentVariableEntry_Material::OnValueChanged()
//{
//
//}