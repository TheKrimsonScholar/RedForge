#pragma once

#include <cstdint>
#include <algorithm>
#include <string>

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/listview.h>
#include <gtkmm/listitem.h>
#include <gtkmm/treelistmodel.h>

#include <glibmm/object.h>

#include "LevelManager.h"

class ModelColumns : public Glib::Object
{
public:
    bool m_col_isExpanded;
    int m_col_id;
    Glib::ustring m_col_name;
    uint32_t m_col_level;
    Entity m_col_entity;

    static Glib::RefPtr<ModelColumns> create(bool col_isExpanded, int col_id, const Glib::ustring& col_name, uint32_t col_level, Entity col_entity)
    {
        return Glib::make_refptr_for_instance<ModelColumns>(new ModelColumns(col_isExpanded, col_id, col_name, col_level, col_entity));
    }

protected:
    ModelColumns(bool col_isExpanded, int col_id, const Glib::ustring& col_name, uint32_t col_level, Entity col_entity)
        : m_col_isExpanded(col_isExpanded), m_col_id(col_id), m_col_name(col_name), m_col_level(col_level), m_col_entity(col_entity)
    {

    }
};

class HierarchicalEntityList : public Gtk::ScrolledWindow
{
private:
    Gtk::ListView m_ListView;

    Glib::RefPtr<Gtk::TreeListModel> m_TreeListModel;

    std::function<void(const std::vector<Entity>&)> onSelectionChanged = nullptr;

    // The saved expand state of entities. Invalid entities reperesent levels and are considered to be always expanded.
    std::unordered_map<Entity, bool> expandedEntities = { { {}, true } };
    std::unordered_map<Entity, Glib::RefPtr<ModelColumns>> entityModelData = {};
    std::unordered_map<Entity, Glib::RefPtr<Gtk::TreeListRow>> entityRows = {};

    std::queue<Entity> entitiesToRefresh;

    bool isRefreshing = false;

    Glib::RefPtr<Gtk::TreeListRow> draggedRow;

public:
    HierarchicalEntityList();
    ~HierarchicalEntityList() override;

    void Initialize();

    void SetOnSelectionChanged(std::function<void(const std::vector<Entity>&)> callback) { onSelectionChanged = callback; };

protected:
    // Signal handlers:
    void on_button_quit();

    void on_setup_list_item(const Glib::RefPtr<Gtk::ListItem>& list_item);
    void on_bind_list_item(const Glib::RefPtr<Gtk::ListItem>& list_item);
    void on_unbind_list_item(const Glib::RefPtr<Gtk::ListItem>& list_item);

    Glib::RefPtr<Gio::ListModel> create_model(const Glib::RefPtr<Glib::ObjectBase>& item = {});

    void RefreshHierarchy(const Entity& modifiedEntity);
    void RefreshNextEntityInQueue();

    void SaveChildExpandStates(const Glib::RefPtr<ModelColumns>& entry);
};

//#pragma once
//
//#include "LevelManager.h"
//
//#include "gtkmm/treeview.h"
//#include "gtkmm/treestore.h"
//#include <gtkmm/dragsource.h>
//#include <gtkmm/droptarget.h>
//
//#include "DebugMacros.h"
//
//class HierarchicalEntityList : public Gtk::TreeView
//{
//private:
//    Glib::RefPtr<Gtk::TreeStore> m_tree_store;
//    std::unordered_map<Entity, Gtk::TreeModel::iterator> entityEntries;
//
//    Entity selectedEntity = {};
//
//    // Define your column structure
//    class ModelColumns : public Gtk::TreeModel::ColumnRecord
//    {
//    public:
//        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
//        Gtk::TreeModelColumn<int> m_col_id;
//        Gtk::TreeModelColumn<Entity> m_col_entity;
//
//    public:
//        ModelColumns()
//        {
//            add(m_col_name);
//            add(m_col_id);
//            add(m_col_entity);
//        }
//    };
//
//    ModelColumns m_columns;
//
//public:
//    HierarchicalEntityList()
//    {
//        // Create the tree store
//        m_tree_store = Gtk::TreeStore::create(m_columns);
//        set_model(m_tree_store);
//
//        // Add columns to the tree view
//        append_column("Name", m_columns.m_col_name);
//
//        // Enable drag and drop
//        setup_drag_and_drop();
//
//        // Add some sample data
//        // Don't do it here; wait until engine is initialized
//        //populate_tree();
//    }
//
//    void UpdateHierarchy()
//    {
//        populate_tree();
//    };
//
//private:
//    void setup_drag_and_drop()
//    {
//        // GTK4: Enable reordering is still the simplest approach
//        set_reorderable(true);
//
//        // For more advanced drag-and-drop in GTK4, you'd use:
//        // 1. Gtk::DragSource for drag operations
//        // 2. Gtk::DropTarget for drop operations
//
//        // Example of custom drag source:
//        auto drag_source = Gtk::DragSource::create();
//        drag_source->set_actions(Gdk::DragAction::MOVE);
//
//        // Connect drag source signals
//        drag_source->signal_prepare().connect([this](double x, double y) -> Glib::RefPtr<Gdk::ContentProvider> {
//            LOG("Prepare");
//            return on_drag_prepare(x, y);
//            }, false);
//
//        drag_source->signal_drag_begin().connect([this](const Glib::RefPtr<Gdk::Drag>& drag) {
//            LOG("Begin");
//            on_drag_begin(drag);
//            });
//
//        get_selection()->signal_changed().connect([this]()
//            {
//                Glib::RefPtr<Gtk::TreeSelection> selection = get_selection();
//                if(Gtk::TreeModel::iterator iter = selection->get_selected())
//                    selectedEntity = (*iter)[m_columns.m_col_entity];
//
//                LOG("Selection Changed: {}", LevelManager::GetEntityName(selectedEntity));
//            });
//        m_tree_store->signal_row_inserted().connect([this](const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it)
//            {
//                Entity entity = (*it)[m_columns.m_col_entity];
//                // If the inserted entity is the same as the one selected, this is an entity move event
//                if(selectedEntity.IsValid() && entity == selectedEntity)
//                {
//                    Entity parent = {};
//                    // If not at root level
//                    if(path.size() > 1)
//                    {
//                        // This entity has a parent
//                        Gtk::TreeModel::Path parent_path = path;
//                        parent_path.up(); // Go to parent path
//
//                        if(Gtk::TreeModel::iterator parentEntry = m_tree_store->get_iter(parent_path))
//                            parent = (*parentEntry)[m_columns.m_col_entity];
//                    }
//
//                    uint32_t siblingIndex = 0;
//                    Gtk::TreeNodeChildren siblings = it->parent() ? it->parent()->children() : m_tree_store->children(); // Default to root children if no parent exists
//                    // Calculate sibling index by counting preceding siblings
//                    for(Gtk::TreeNodeChildren::iterator sibling = siblings.begin(); sibling != siblings.end(); sibling++)
//                    {
//                        if(sibling == it)
//                            break;
//
//                        siblingIndex++;
//                    }
//
//                    LOG("Entity: {}, New Parent: {}, New Sibling Index: {}", LevelManager::GetEntityName(entity), LevelManager::GetEntityName(parent), siblingIndex);
//
//                    //// Update the entity system
//                    //if(parent.IsValid())
//                    //    LevelManager::SetEntityParent(entity, parent);
//
//                    //LevelManager::SetEntitySiblingIndex(entity, siblingIndex);
//                }
//
//                auto fresh_iter = m_tree_store->get_iter(path);
//                if(fresh_iter)
//                {
//                    Entity ientity = (*fresh_iter)[m_columns.m_col_entity];
//                    // ... rest of logic
//                    LOG("Entity Inserted: {}", LevelManager::GetEntityName(ientity));
//                }
//            });
//        m_tree_store->signal_row_deleted().connect([](const Gtk::TreeModel::Path& path)
//            {
//                //Entity entity = path
//
//                //selectedEntity = entity;
//
//                LOG("Row Deleted {}", (std::string) path.to_string());
//            });
//        m_tree_store->signal_rows_reordered().connect([](const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it, int* i)
//            {
//                LOG("Rows Reordered");
//            });
//        m_tree_store->signal_row_changed().connect([](const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it)
//            {
//                LOG("Row Changed");
//            });
//
//        add_controller(drag_source);
//
//        // Example of drop target:
//        auto drop_target = Gtk::DropTarget::create(G_TYPE_STRING, Gdk::DragAction::MOVE);
//
//        drop_target->signal_drop().connect([this](const Glib::ValueBase& value, double x, double y) -> bool {
//            LOG("Drop");
//            return on_drop(value, x, y);
//            }, false);
//
//        add_controller(drop_target);
//    }
//
//    Glib::RefPtr<Gdk::ContentProvider> on_drag_prepare(double x, double y)
//    {
//        // Get the item being dragged
//        auto selection = get_selection();
//        if (auto iter = selection->get_selected()) {
//            Glib::ustring name = (*iter)[m_columns.m_col_name];
//
//            // Create content provider with the dragged data
//            auto value = Glib::Value<Glib::ustring>();
//            value.init(G_TYPE_STRING);
//            value.set(name);
//
//            return Gdk::ContentProvider::create(value);
//        }
//        return {};
//    }
//
//    void on_drag_begin(const Glib::RefPtr<Gdk::Drag>& drag)
//    {
//        // Optional: Set custom drag icon
//        // auto paintable = ...; // Create your custom icon
//        // drag->set_hotspot(0, 0);
//    }
//
//    bool on_drop(const Glib::ValueBase& value, double x, double y)
//    {
//        // Handle the drop operation - GTK4 way
//        if (value.gobj() && G_VALUE_HOLDS_STRING(value.gobj())) {
//            const char* dropped_text = g_value_get_string(value.gobj());
//            if (dropped_text) {
//                Glib::ustring text(dropped_text);
//
//                // Determine drop location and perform the move
//                // This is where you'd implement your custom drop logic
//
//                return true; // Accept the drop
//            }
//        }
//        return false; // Reject the drop
//    }
//
//    void populate_tree()
//    {
//        //// Add parent items
//        //auto parent1 = m_tree_store->append();
//        //(*parent1)[m_columns.m_col_name] = "Parent 1";
//        //(*parent1)[m_columns.m_col_id] = 1;
//
//        //// Add children to parent1
//        //auto child1 = m_tree_store->append(parent1->children());
//        //(*child1)[m_columns.m_col_name] = "Child 1.1";
//        //(*child1)[m_columns.m_col_id] = 11;
//
//        //auto child2 = m_tree_store->append(parent1->children());
//        //(*child2)[m_columns.m_col_name] = "Child 1.2";
//        //(*child2)[m_columns.m_col_id] = 12;
//
//        //// Add another parent
//        //auto parent2 = m_tree_store->append();
//        //(*parent2)[m_columns.m_col_name] = "Parent 2";
//        //(*parent2)[m_columns.m_col_id] = 2;
//
//        //auto child3 = m_tree_store->append(parent2->children());
//        //(*child3)[m_columns.m_col_name] = "Child 2.1";
//        //(*child3)[m_columns.m_col_id] = 21;
//
//        LevelManager::ForEachEntity([this](const Entity& entity)
//            {
//                // TEMP
//                static uint32_t index = 0;
//
//                Entity parent = LevelManager::GetEntityParent(entity);
//
//                Gtk::TreeModel::iterator* parentEntry = nullptr;
//                // If parent has been given an entry, keep note of it
//                // Note that the ordering of ForEachEntity() ensures that parents are processed before their children
//                if(entityEntries.find(parent) != entityEntries.end())
//                    parentEntry = &entityEntries.at(parent);
//
//                // If parent entry exists, use it as the new entry's parent
//                if(parentEntry)
//                    entityEntries.emplace(entity, m_tree_store->append((*parentEntry)->children()));
//                // Otherwise append this entry to the top level
//                else
//                    entityEntries.emplace(entity, m_tree_store->append());
//                Gtk::TreeModel::iterator& entityEntry = entityEntries.at(entity);
//
//                // Set the properties of this entity entry
//                (*entityEntry)[m_columns.m_col_name] = LevelManager::GetEntityName(entity);
//                (*entityEntry)[m_columns.m_col_id] = index++;
//                (*entityEntry)[m_columns.m_col_entity] = entity;
//
//                /*Gtk::ListBoxRow* row = Gtk::manage(new Gtk::ListBoxRow());
//                Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
//
//                Gtk::Label* entityLabel = Gtk::manage(new Gtk::Label(LevelManager::GetEntityName(entity)));
//                entityLabel->set_margin_start(CHILD_DEPTH_PADDING * (LevelManager::GetEntityDepth(entity) - 1));
//                Gtk::Box* spacer = Gtk::manage(new Gtk::Box(Gtk::Orientation::HORIZONTAL));
//                spacer->set_hexpand(true);
//
//                Gtk::Button* destroyButton = Gtk::manage(new Gtk::Button("X"));
//                destroyButton->set_halign(Gtk::Align::END);
//                destroyButton->set_tooltip_text("Destroy this entity");
//                destroyButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &HierarchyPanel::DestroyEntity), entity), false);
//
//                box->append(*entityLabel);
//                box->append(*spacer);
//                box->append(*destroyButton);*/
//
//                //Glib::RefPtr<Gtk::GestureClick> click = Gtk::GestureClick::create();
//                //Gtk::GestureDrag
//                //click->set_button(0); // Respond to all mouse buttons
//                //click->signal_pressed().connect([entity](int, double, double) { LOG("Pressed {}", LevelManager::GetEntityName(entity)); }, false);
//                //click->signal_released().connect([entity](int, double, double) { LOG("Released {}", LevelManager::GetEntityName(entity)); }, false);
//                //box->add_controller(click);
//
//                //row->set_child(*box);
//
//                //entityList.append(*row);
//                //entityRows.emplace(row, entity);
//            });
//    }
//};