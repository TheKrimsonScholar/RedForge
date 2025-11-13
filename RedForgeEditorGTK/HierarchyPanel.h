//#pragma once
//
//#include "EditorPanel.h"
//
//#include "gtkmm/frame.h"
//#include "gtkmm/listbox.h"
//#include "gtkmm/button.h"
//
//#include "HierarchicalEntityList.h"
//
//#include "InspectorPanel.h"
//
//class HierarchyPanel : public EditorPanel
//{
//private:
//	Gtk::Button createEntityButton;
//
//	//Gtk::ListBox entityList;
//	//std::unordered_map<Gtk::ListBoxRow*, Entity> entityRows;
//
//	HierarchicalEntityList entityList;
//
//	std::vector<Entity> selectedEntities;
//
//public:
//	InspectorPanel* inspector = nullptr;
//
//public:
//	HierarchyPanel();
//	~HierarchyPanel();
//
//	void Initialize();
//	void UpdateHierarchy();
//
//private:
//	void CreateEntity();
//	void DestroyEntity(Entity entity);
//
//	void OnRowActivated(Gtk::ListBoxRow* row);
//	void OnSelectedRowsChanged();
//
//	void DestroyHierarchy();
//};