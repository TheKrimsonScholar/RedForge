#pragma once

#include "EditorPanel.h"

#include "gtkmm/frame.h"
#include "gtkmm/listbox.h"
#include "gtkmm/button.h"

#include "InspectorPanel.h"

class HierarchyPanel : public EditorPanel
{
private:
	Gtk::Button createEntityButton;

	Gtk::ListBox entityList;
	std::unordered_map<Gtk::ListBoxRow*, Entity> entityRows;

	Entity selectedEntity = {};

public:
	InspectorPanel* inspector = nullptr;

public:
	HierarchyPanel();
	~HierarchyPanel();

	void UpdateHierarchy();

private:
	void CreateEntity();
	void DestroyEntity(Entity entity);

	void OnRowActivated(Gtk::ListBoxRow* row);
	void OnSelectedRowsChanged();

	void DestroyHierarchy();
};