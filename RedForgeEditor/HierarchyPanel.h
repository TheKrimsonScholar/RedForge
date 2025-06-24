#pragma once

#include "gtkmm/frame.h"
#include "gtkmm/listbox.h"
#include "gtkmm/button.h"
#include "gtkmm/scrolledwindow.h"

#include "InspectorWindow.h"

class HierarchyPanel : public Gtk::ScrolledWindow
{
private:
	Gtk::Box contentArea;

	Gtk::Button createEntityButton;

	Gtk::ListBox entityList;
	std::unordered_map<Gtk::ListBoxRow*, Entity> entityRows;

	Entity selectedEntity = INVALID_ENTITY;

public:
	InspectorWindow* inspector = nullptr;

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