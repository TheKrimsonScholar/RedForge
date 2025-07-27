#include "HierarchyPanel.h"

#include "LevelManager.h"

#include <gtkmm/label.h>

#include "DebugMacros.h"

HierarchyPanel::HierarchyPanel() : EditorPanel("Hierarchy"), 
	createEntityButton("New Entity"), entityList()
{
	entityList.add_css_class("entity-list");

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
	static const int CHILD_DEPTH_PADDING = 32;

	DestroyHierarchy();

	LevelManager::ForEachEntity([this](const Entity& entity)
		{
			Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
			Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
		
			Gtk::Label* entityLabel = Gtk::manage(new Gtk::Label(LevelManager::GetEntityName(entity)));
			entityLabel->set_margin_start(CHILD_DEPTH_PADDING * (LevelManager::GetEntityDepth(entity) - 1));
			Gtk::Box* spacer = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
			spacer->set_hexpand(true);

			Gtk::Button* destroyButton = Gtk::manage(new Gtk::Button("X"));
			destroyButton->set_halign(Gtk::Align::END);
			destroyButton->set_tooltip_text("Destroy this entity");
			destroyButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &HierarchyPanel::DestroyEntity), entity), false);
		
			box->append(*entityLabel);
			box->append(*spacer);
			box->append(*destroyButton);

			row->set_child(*box);

			entityList.append(*row);
			entityRows.emplace(row, entity);
		});
}

void HierarchyPanel::CreateEntity()
{
	LevelManager::CreateEntity("", selectedEntity);

	UpdateHierarchy();
}
void HierarchyPanel::DestroyEntity(Entity entity)
{
	if(selectedEntity == entity)
	{
		inspector->ResetTarget();
		selectedEntity = {};
	}

	LevelManager::DestroyEntity(entity);

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
	// Invalidate selected entity
	selectedEntity = {};

	while(auto child = entityList.get_first_child())
		entityList.remove(*child);
	entityRows.clear();
}