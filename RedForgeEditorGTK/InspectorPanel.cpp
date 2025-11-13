//#include "InspectorPanel.h"
//
//#include "LevelManager.h"
//#include "DebugMacros.h"
//
//#include <gtkmm/button.h>
//
//#include "SearchFieldPopup.h"
//
//#include "TransformComponent.h"
//
//InspectorPanel::InspectorPanel() : EditorPanel("Inspector"),
//	entityLabel("None"), componentsList()
//{
//	componentsList.add_css_class("inspector-list");
//
//	contentArea.append(entityLabel);
//	contentArea.append(componentsList);
//}
//InspectorPanel::~InspectorPanel()
//{
//
//}
//
//void InspectorPanel::SetTarget(Entity entity)
//{
//	entityLabel.set_label(LevelManager::GetEntityName(entity));
//
//	// Clear any existing component entries
//	while(auto child = componentsList.get_first_child())
//		componentsList.remove(*child);
//
//	std::unordered_map<void*, std::type_index> components = EntityManager::GetAllComponents(entity);
//	std::vector<std::type_index> registeredComponentTypes = GetRegisteredComponentsList();
//	// List this entity's components in order by type
//	for(std::type_index componentType : registeredComponentTypes)
//		for(auto& component : components)
//		{
//			if(component.second != componentType)
//				continue;
//
//			Gtk::Box* componentEntry = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
//			componentEntry->add_css_class("component-entry");
//			componentEntry->set_halign(Gtk::Align::FILL);
//			
//			ComponentEntry* newEntry = Gtk::manage(new ComponentEntry(entity, component.second, component.first));
//			Gtk::Box* spacer = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
//			spacer->set_hexpand(true);
//			
//			// Create remove component button
//			Gtk::Button* removeComponentButton = Gtk::manage(new Gtk::Button("X"));
//			removeComponentButton->set_halign(Gtk::Align::END);
//			removeComponentButton->signal_clicked().connect([this, entity, componentType]()
//				{
//					EntityManager::RemoveComponentOfType(entity, componentType);
//
//					// Refresh the inspector
//					SetTarget(entity);
//				});
//			
//			componentEntry->append(*newEntry);
//			componentEntry->append(*spacer);
//			componentEntry->append(*removeComponentButton);
//			
//			componentsList.append(*componentEntry);
//		}
//
//	/* Search field for adding new components to the entity */
//
//	std::vector<std::string> componentNames;
//	// Get the names of all registered component types
//	for(std::type_index componentTypeIndex : GetRegisteredComponentsList())
//		componentNames.push_back(GET_COMPONENT_NAME(componentTypeIndex));
//
//	SearchFieldPopup* addComponentField = Gtk::manage(new SearchFieldPopup("Add Component..."));
//	addComponentField->SetSelectionList(componentNames);
//	addComponentField->signal_selection_made().connect([this, entity](uint32_t index, const Glib::ustring& selectedItem)
//		{
//			EntityManager::AddComponentOfType(entity, GetRegisteredComponentsList()[index]);
//
//			// Refresh the inspector
//			SetTarget(entity);
//		});
//	componentsList.append(*addComponentField);
//}
//void InspectorPanel::SetTargets(const std::vector<Entity>& entities)
//{
//	entityLabel.set_label("Multiple entities selected (This is not yet supported).");
//
//	// Clear any existing component entries
//	while(auto child = componentsList.get_first_child())
//		componentsList.remove(*child);
//}
//
//void InspectorPanel::ResetTarget()
//{
//	entityLabel.set_label("None");
//
//	// Clear any existing component entries
//	while(auto child = componentsList.get_first_child())
//		componentsList.remove(*child);
//}