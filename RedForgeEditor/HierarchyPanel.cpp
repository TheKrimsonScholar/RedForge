#include "HierarchyPanel.h"

#include "EntityManager.h"

#include <gtkmm/label.h>

#include "DebugMacros.h"

HierarchyPanel::HierarchyPanel() : Gtk::ScrolledWindow(), // "Hierarchy"
	contentArea(Gtk::Orientation::VERTICAL), createEntityButton("New Entity"), entityList()
{
	set_child(contentArea);

	createEntityButton.set_tooltip_text("Create a new entity");
	createEntityButton.signal_clicked().connect(sigc::mem_fun(*this, &HierarchyPanel::CreateEntity), false);

	entityList.set_selection_mode(Gtk::SelectionMode::MULTIPLE);
	entityList.set_activate_on_single_click(false);

	entityList.signal_row_activated().connect(sigc::mem_fun(*this, &HierarchyPanel::OnRowActivated), false);
	entityList.signal_selected_rows_changed().connect(sigc::mem_fun(*this, &HierarchyPanel::OnSelectedRowsChanged), false);
	
	contentArea.append(createEntityButton);
	contentArea.append(entityList);
}
HierarchyPanel::~HierarchyPanel()
{

}

void HierarchyPanel::UpdateHierarchy()
{
	DestroyHierarchy();

	for(Entity e = 0; e < EntityManager::GetLastEntity(); e++)
	{
		if(!EntityManager::IsEntityValid(e))
			continue;

		Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
		Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
		
		Gtk::Label* entityLabel = Gtk::manage(new Gtk::Label("Entity" + std::to_string(e)));
		Gtk::Box* spacer = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
		spacer->set_hexpand(true);

		Gtk::Button* destroyButton = Gtk::manage(new Gtk::Button("X"));
		destroyButton->set_halign(Gtk::Align::END);
		destroyButton->set_tooltip_text("Destroy this entity");
		destroyButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &HierarchyPanel::DestroyEntity), e), false);
		
		box->append(*entityLabel);
		box->append(*spacer);
		box->append(*destroyButton);

		row->set_child(*box);

		entityList.append(*row);
		entityRows.emplace(row, e);
	}
}

void HierarchyPanel::CreateEntity()
{
	EntityManager::CreateEntity();

	UpdateHierarchy();
}
void HierarchyPanel::DestroyEntity(Entity entity)
{
	if(selectedEntity == entity)
	{
		inspector->ResetTarget();
		selectedEntity = INVALID_ENTITY;
	}

	EntityManager::DestroyEntity(entity);

	UpdateHierarchy();
}

void HierarchyPanel::OnRowActivated(Gtk::ListBoxRow* row)
{
	//LOG("Activated %i", row->get_index());

	selectedEntity = entityRows[row];

	if(inspector)
		inspector->SetTarget(entityRows[row]);
}
void HierarchyPanel::OnSelectedRowsChanged()
{
	//LOG("Selection Changed");
}

void HierarchyPanel::DestroyHierarchy()
{
	selectedEntity = INVALID_ENTITY;

	while(auto child = entityList.get_first_child())
		entityList.remove(*child);
	entityRows.clear();
}