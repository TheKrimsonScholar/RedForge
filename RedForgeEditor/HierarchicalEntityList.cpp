#include "HierarchicalEntityList.h"

#include <iostream>

#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/treeexpander.h>
#include <gtkmm/signallistitemfactory.h>
#include <gtkmm/multiselection.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/dragsource.h>
#include <gtkmm/droptarget.h>

#include <glibmm/signalproxy.h>

#include <giomm.h>

HierarchicalEntityList::HierarchicalEntityList()
{
    // Add the ColumnView, inside a ScrolledWindow, with the button underneath:
    set_child(m_ListView);

    // Only show the scrollbars when they are necessary:
    set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    set_expand();
}
HierarchicalEntityList::~HierarchicalEntityList()
{
    // Unbind events
    LevelManager::GetOnEntityCreated()->RemoveUnique(EventCallback(this, &HierarchicalEntityList::RefreshHierarchy));
    LevelManager::GetOnEntityDestroyed()->RemoveUnique(EventCallback(this, &HierarchicalEntityList::RefreshHierarchy));
}

void HierarchicalEntityList::Initialize()
{
    // Create the root model:
    auto root = create_model();

    // Set list model and selection model.
    // passthrough must be false when Gtk::TreeExpander is used in the view.
    m_TreeListModel = Gtk::TreeListModel::create(root,
        sigc::mem_fun(*this, &HierarchicalEntityList::create_model),
        /* passthrough */ false, /* autoexpand */ false);
    auto selection_model = Gtk::MultiSelection::create(m_TreeListModel);
    selection_model->signal_selection_changed().connect([this, selection_model](guint k1, guint k2)
        {
            std::cout << "{" << k1 << ", " << k2 << "}" << std::endl;

            if(!onSelectionChanged)
                return;

            const Gtk::Bitset& selection = *selection_model->get_selection().get();
            
            std::vector<Entity> selectedEntities;
            for(auto selectedItem : selection)
            {
                auto row = dynamic_pointer_cast<Gtk::TreeListRow>(selection_model->get_object(selectedItem));
                auto col = dynamic_pointer_cast<ModelColumns>(row->get_item());

                std::cout << selectedItem << " " << col->m_col_name << std::endl;

                // Only add valid entities (not entries that represent levels)
                if(col->m_col_entity.IsValid())
                    selectedEntities.push_back(col->m_col_entity);
            }

            onSelectionChanged(selectedEntities);
        });
    m_ListView.set_model(selection_model);
    m_ListView.add_css_class("data-table"); // high density table

    auto factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect(
        sigc::mem_fun(*this, &HierarchicalEntityList::on_setup_list_item));
    factory->signal_bind().connect(
        sigc::mem_fun(*this, &HierarchicalEntityList::on_bind_list_item));
    factory->signal_unbind().connect(
        sigc::mem_fun(*this, &HierarchicalEntityList::on_unbind_list_item));
    m_ListView.set_factory(factory);

    // Set callbacks for when entities are created and destroyed
    // The hierarchy will be refreshed in both cases, and if a selected entity is destroyed that entity will be deselected
    LevelManager::GetOnEntityCreated()->AddUnique(EventCallback(this, &HierarchicalEntityList::RefreshHierarchy));
    LevelManager::GetOnEntityDestroyed()->AddUnique(EventCallback(this, &HierarchicalEntityList::RefreshHierarchy));
}

void HierarchicalEntityList::on_button_quit()
{
    set_visible(false);
}

void HierarchicalEntityList::on_setup_list_item(const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    // Each ListItem contains a TreeExpander, which contains a Label.
    // The Label shows the ModelColumns::m_col_id. That's done in on_bind_id(). 
    auto expander = Gtk::make_managed<Gtk::TreeExpander>();
    expander->set_indent_for_depth();
    auto label = Gtk::make_managed<Gtk::Label>("", Gtk::Align::START);

    box->append(*expander);
    box->append(*label);

    list_item->set_child(*box);
}
void HierarchicalEntityList::on_bind_list_item(const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    // When TreeListModel::property_passthrough() is false, ListItem::get_item()
    // is a TreeListRow. TreeExpander needs the TreeListRow.
    // The ModelColumns item is returned by TreeListRow::get_item().
    auto row = std::dynamic_pointer_cast<Gtk::TreeListRow>(list_item->get_item());
    if(!row)
        return;

    auto col = std::dynamic_pointer_cast<ModelColumns>(row->get_item());
    if(!col)
        return;
    entityRows[col->m_col_entity] = row;

    auto box = dynamic_cast<Gtk::Box*>(list_item->get_child());
    if(!box)
        return;
    std::vector<Gtk::Widget*> widgets = box->get_children();

    auto expander = dynamic_cast<Gtk::TreeExpander*>(widgets[0]);
    if(!expander)
        return;
    expander->set_list_row(row);
    // If entity is invalid, this entry represents a level and the expand arrow is hidden
    //expander->set_hide_expander(!col->m_col_entity.IsValid());

    auto nameLabel = dynamic_cast<Gtk::Label*>(widgets[1]);
    if(!nameLabel)
        return;
    nameLabel->set_text(col->m_col_name);

    // Connect click gesture to intercept expand/collapse clicks
    auto gesture = Gtk::GestureClick::create();
    expander->add_controller(gesture);

    gesture->signal_pressed().connect([this, expander, row, col](int, double, double)
        {
            /* Called *near* the dropdown arrow; when this is called, there's only a *chance* that an expand/collapse event is being started */

            bool initialExpanded = row->get_expanded();
            Glib::signal_timeout().connect_once([this, row, col, initialExpanded]()
                {
                    if (row->get_expanded() != initialExpanded)
                    {
                        col->m_col_isExpanded = row->get_expanded();
                        expandedEntities[col->m_col_entity] = col->m_col_isExpanded;
                    }
                }, 100);

            // If row is currently expanded, it's possible it's going to collapse - save all child expand states beforehand
            // This happens even if the dropdown wasn't clicked, but saving too often shouldn't be an issue so this is acceptable
            if (row->get_expanded())
            {
                SaveChildExpandStates(col);

                std::cout << row->get_expanded() << std::endl;
            }
        });

    Glib::signal_timeout().connect([this, row, col]() -> bool
        {
            if(expandedEntities.find(col->m_col_entity) != expandedEntities.end())
                row->set_expanded(expandedEntities[col->m_col_entity]);

            return false;
        }, 100);


    row->property_expanded().signal_changed().connect(
        [this, list_item, row, col]()
        {
            //std::cout << "expanded " << col->m_col_name << " " << col->m_col_entity.index << ":" << col->m_col_entity.generation << " " << row->get_expanded() << " POSITION:" << row->get_parent() << std::endl;

            // Only update current expand state if the row is visible (parent is non-null)
            // Note that this means top-level rows (levels) will never have their expand state saved since their parent is null even when they are active
            // This shouldn't matter since the top-level rows will never be hidden, so their expand state is never lost
            if(row->get_parent())
                col->m_col_isExpanded = row->get_expanded();
        });

    // ---- Drag source ----
    auto drag_source = Gtk::DragSource::create();
    drag_source->set_actions(Gdk::DragAction::MOVE);
    drag_source->signal_prepare().connect(
        [this, row, col](double x, double y) -> Glib::RefPtr<Gdk::ContentProvider>
        {
            Glib::Value<Glib::ustring> val;
            val.init(Glib::Value<Glib::ustring>::value_type());
            val.set(col->m_col_name);

            draggedRow = row;

            return Gdk::ContentProvider::create(val);
        }, false);
    nameLabel->add_controller(drag_source);

    // ---- Drop target ----
    auto drop_target = Gtk::DropTarget::create(
        Glib::Value<Glib::ustring>::value_type(), // accept text drops
        Gdk::DragAction::MOVE);
    drop_target->signal_drop().connect(
        [this, row, col, list_item](const Glib::ValueBase& value, double x, double y) -> bool
        {
            static const float LIST_ITEM_VERTICAL_PADDING = 0.25f;

            Glib::Value<Glib::ustring> entity;
            entity.init(value.gobj());

            Glib::RefPtr<ModelColumns> draggedColumn = std::dynamic_pointer_cast<ModelColumns>(draggedRow->get_item());

            Entity draggedEntity = draggedColumn->m_col_entity;
            Entity destinationEntity = col->m_col_entity;

            int listItemHeight = list_item->get_child()->get_height();
            bool success = false;
            if(y < listItemHeight * LIST_ITEM_VERTICAL_PADDING)
            {
                //std::cout << "Drop received REORDER BEFORE: " << entity.get() << "{" << draggedEntity.index << ", " << draggedEntity.generation << "}" << " x:" << x << " y:" << y << " height:" << listItemHeight << std::endl;

                success = LevelManager::MoveEntityBefore(draggedEntity, destinationEntity);
            }
            else if(y > listItemHeight * (1 - LIST_ITEM_VERTICAL_PADDING))
            {
                //std::cout << "Drop received REORDER AFTER: " << entity.get() << "{" << draggedEntity.index << ", " << draggedEntity.generation << "}" << " x:" << x << " y:" << y << " height:" << listItemHeight << std::endl;

                success = LevelManager::MoveEntityAfter(draggedEntity, destinationEntity);
            }
            else
            {
                //std::cout << "Drop received REPARENT: " << entity.get() << "{" << draggedEntity.index << ", " << draggedEntity.generation << "}" << " x:" << x << " y:" << y << " height:" << listItemHeight << std::endl;

                success = LevelManager::SetEntityParent(draggedEntity, destinationEntity);
            }

            if(!success)
                return false;

            /* Refresh relevant entries by re-expanding their parent rows */

            //RefreshHierarchy(draggedEntity);
            //RefreshHierarchy(destinationEntity);
            Glib::signal_timeout().connect_once([this, row, col, draggedColumn]()
                {
                    Glib::RefPtr<Gtk::TreeListRow> destinationParent = row->get_parent();
                    SaveChildExpandStates(col);
                    Glib::signal_timeout().connect_once([this, row, col, draggedColumn, destinationParent]()
                        {
                            destinationParent->set_expanded(false);
                            Glib::signal_timeout().connect_once([destinationParent]() { destinationParent->set_expanded(true); }, 10);

                            // If the dragged row is disconnected (parent is null), then it was already somewhere under the parent of the destination row; don't need to save or refresh again
                            if(Glib::RefPtr<Gtk::TreeListRow> draggedParent = draggedRow->get_parent())
                            {
                                SaveChildExpandStates(draggedColumn);
                                draggedParent->set_expanded(false);
                                draggedParent->set_expanded(true);
                            }
                        }, 10);
                }, 10);

            // Reconstruct full tree model
            //Glib::signal_timeout().connect_once([this, row, col, draggedColumn]()
            //    {
            //        auto root = create_model();
            //        m_TreeListModel = Gtk::TreeListModel::create(root,
            //            sigc::mem_fun(*this, &ExampleWindow::create_model),
            //            /* passthrough */ false, /* autoexpand */ false);
            //        auto selection_model = Gtk::MultiSelection::create(m_TreeListModel);
            //        m_ColumnView.set_model(selection_model);
            //    }, 0);

            return true;
        }, false);
    nameLabel->add_controller(drop_target);
}
void HierarchicalEntityList::on_unbind_list_item(const Glib::RefPtr<Gtk::ListItem>& list_item)
{
    auto row = std::dynamic_pointer_cast<Gtk::TreeListRow>(list_item->get_item());
    if(!row)
        return;

    auto col = std::dynamic_pointer_cast<ModelColumns>(row->get_item());
    if(!col)
        return;
    entityRows.erase(col->m_col_entity);
}

Glib::RefPtr<Gio::ListModel> HierarchicalEntityList::create_model(
    const Glib::RefPtr<Glib::ObjectBase>& item)
{
    auto col = std::dynamic_pointer_cast<ModelColumns>(item);
    auto result = Gio::ListStore<ModelColumns>::create();

    // At the root level, list the active levels
    if(!col)
    {
        Entity rootEntity = {};

        entityModelData.emplace(rootEntity, ModelColumns::create(true, 0, "Level 0", 0, {}));

        result->append(entityModelData.at(rootEntity));
    }
    // Otherwise, list the child entities
    else
    {
        Entity currentChild = LevelManager::GetEntityFirstChild(col->m_col_entity);
        while(currentChild.IsValid())
        {
            // If the entity's model data isn't registered yet, make a new entry
            if(entityModelData.find(currentChild) == entityModelData.end())
                entityModelData.emplace(currentChild, ModelColumns::create(false, 1, LevelManager::GetEntityName(currentChild), 0, currentChild));
        
            // Use existing saved model data to preserve state
            result->append(entityModelData.at(currentChild));

            // Go to next child
            currentChild = LevelManager::GetEntityNextSibling(currentChild);
        }
    }

    // If result is empty, it's a leaf in the tree, i.e. an item without children.
    // Returning an empty RefPtr (not a RefPtr with an empty Gio::ListModel)
    // signals that the item is not expandable.
    return (result->get_n_items() > 0) ? result : Glib::RefPtr<Gio::ListStore<ModelColumns>>();
}

void HierarchicalEntityList::RefreshHierarchy(const Entity& modifiedEntity)
{
    const Entity& parent = LevelManager::GetEntityParent(modifiedEntity);

    entitiesToRefresh.push(parent);
    // If already refreshing, let the current refresh instance pick up this entity when it's ready
    if(isRefreshing)
        return;

    RefreshNextEntityInQueue();
}
void HierarchicalEntityList::RefreshNextEntityInQueue()
{
    if(entitiesToRefresh.empty())
        return;

    const Entity& parent = entitiesToRefresh.front();

    if(entityRows.find(parent) == entityRows.end() || entityModelData.find(parent) == entityModelData.end())
    {
        entitiesToRefresh.pop();
        RefreshNextEntityInQueue();
        return;
    }

    Glib::RefPtr<Gtk::TreeListRow> parentRow = entityRows.at(parent);
    Glib::RefPtr<ModelColumns> parentCol = entityModelData.at(parent);

    isRefreshing = true;
    Glib::signal_timeout().connect_once([this, parentRow, parentCol]()
        {
            SaveChildExpandStates(parentCol);
            Glib::signal_timeout().connect_once([this, parentRow, parentCol]()
                {
                    parentRow->set_expanded(false);
                    Glib::signal_timeout().connect_once([this, parentRow]()
                        {
                            parentRow->set_expanded(true);

                            entitiesToRefresh.pop();
                            if(entitiesToRefresh.empty())
                                isRefreshing = false;
                            else
                                RefreshNextEntityInQueue();
                        }, 10);
                }, 10);
        }, 10);
}

void HierarchicalEntityList::SaveChildExpandStates(const Glib::RefPtr<ModelColumns>& entry)
{
    expandedEntities[entry->m_col_entity] = entry->m_col_isExpanded;

    // For each child entity, save expanded state
    LevelManager::ForEachEntity([this](const Entity& child)
        {
            // If the entity hasn't been given model data yet (hasn't appeared in the list yet), don't need to save its state
            if(entityModelData.find(child) == entityModelData.end())
                return;

            // TODO: Also return if entity doesn't currently have a row that is visible (i.e. parent is collapsed)

            expandedEntities[child] = entityModelData.at(child)->m_col_isExpanded;

            //std::cout << "Saved " << entityModelData.at(child)->m_col_isExpanded << " " << entityModelData.at(child)->m_col_name << " " << entityModelData.at(child)->m_col_level << " " << entityModelData.at(child)->m_col_entity.index << ":" << entityModelData.at(child)->m_col_entity.generation << std::endl;
        }, entry->m_col_entity);
}