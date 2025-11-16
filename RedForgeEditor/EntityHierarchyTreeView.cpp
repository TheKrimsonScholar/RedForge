#include "EntityHierarchyTreeView.h"

#include <QKeyEvent>
#include <QTimer>

#include "MainEditorWindow.h"

#include "EditorPaths.h"

EntityHierarchyTreeView::EntityHierarchyTreeView(QWidget* parent) : QTreeView(parent)
{
	model = new EntityHierarchyItemModel(this);
	setModel(model);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setDragDropMode(QAbstractItemView::DragDrop);
	setDefaultDropAction(Qt::MoveAction);
	QObject::connect(selectionModel(), &QItemSelectionModel::selectionChanged,
		[this](const QItemSelection& selected, const QItemSelection& deselected)
		{
			// Add all entities that were selected
			for(const QModelIndex& modelIndex : selected.indexes())
				if(QStandardItem* item = model->itemFromIndex(modelIndex))
				{
					const Entity& entity = item->data(Qt::UserRole).value<Entity>();
					
					selectedEntities.push_back(entity);
				}

			// Remove all entities that were deselected
			for(const QModelIndex& modelIndex : deselected.indexes())
				if(QStandardItem* item = model->itemFromIndex(modelIndex))
				{
					const Entity& entity = item->data(Qt::UserRole).value<Entity>();
					auto it = std::find(selectedEntities.begin(), selectedEntities.end(), entity);
					
					if(it != selectedEntities.end())
						selectedEntities.erase(it);
				}

			// Defer inspector update for next frame to avoid conflicts between simultaneous changes in selection
			QTimer::singleShot(0, this, 
				[this]()
				{
					if(InspectorPanel* inspector = MainEditorWindow::Get()->GetInspectorPanel())
						if(!selectedEntities.empty())
							inspector->SetTarget(selectedEntities[0]);
						else
							inspector->ResetTarget();
				});
		});
}
EntityHierarchyTreeView::~EntityHierarchyTreeView()
{

}

void EntityHierarchyTreeView::InitializeHierarchy()
{
	model->InitializeHierarchy();

	// Bind events for when entities are created and destroyed
	LevelManager::GetOnEntityCreated()->AddUnique(EventCallback(this, &EntityHierarchyTreeView::OnEntityCreated));
	LevelManager::GetOnEntityDestroyed()->AddUnique(EventCallback(this, &EntityHierarchyTreeView::OnEntityDestroyed));

	// Bind event for when level data is modified
	LevelManager::GetOnEntityLevelDataModified()->AddUnique(EventCallback(this, &EntityHierarchyTreeView::OnEntityLevelDataModified));

	// Bind events for move operations
	LevelManager::GetOnEntityReparented()->AddUnique(EventCallback(this, &EntityHierarchyTreeView::OnEntityReparented));
	LevelManager::GetOnEntityMovedBefore()->AddUnique(EventCallback(this, &EntityHierarchyTreeView::OnEntityMovedBefore));
	LevelManager::GetOnEntityMovedAfter()->AddUnique(EventCallback(this, &EntityHierarchyTreeView::OnEntityMovedAfter));
}

void EntityHierarchyTreeView::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key::Key_Delete)
	{
		// Destroy all selected entities (the view will be updated automatically via the event bindings)
		for(const Entity& entity : selectedEntities)
			LevelManager::DestroyEntity(entity);
	}
}

void EntityHierarchyTreeView::OnEntityCreated(const Entity& entity)
{
	model->CreateEntityItem(entity);
}
void EntityHierarchyTreeView::OnEntityDestroyed(const Entity& entity)
{
	// Attempt to destroy all children first
	LevelManager::ForEachEntity_Reversed(
		[this](const Entity& entity)
		{
			if(model->GetEntityItem(entity))
				model->DestroyEntityItem(entity);
		}, entity);
}

void EntityHierarchyTreeView::OnEntityLevelDataModified(const Entity& entity)
{
	if(QStandardItem* entityItem = model->GetEntityItem(entity))
	{
		QIcon icon = LevelManager::GetEntityPrefabPath(entity).empty() ? QICON_FROM_PATH("Entity Hierarchy/Entity") : QICON_FROM_PATH("Entity Hierarchy/Prefab");
		entityItem->setIcon(icon);
	}
}

void EntityHierarchyTreeView::OnEntityReparented(const Entity& entity, const Entity& newParent)
{
	Entity oldParent = LevelManager::GetEntityParent(entity);
	QStandardItem* oldParentItem = oldParent.IsValid() ? model->GetEntityItem(oldParent) : model->invisibleRootItem();
	int oldRow = model->GetEntityItem(entity)->row();

	QList<QStandardItem*> itemsToMove = oldParentItem->takeRow(oldRow);
	
	QStandardItem* newParentItem = newParent.IsValid() ? model->GetEntityItem(newParent) : model->invisibleRootItem();
	newParentItem->appendRows(itemsToMove);
}
void EntityHierarchyTreeView::OnEntityMovedBefore(const Entity& entity, const Entity& newNext)
{
	Entity oldParent = LevelManager::GetEntityParent(entity);
	QStandardItem* oldParentItem = oldParent.IsValid() ? model->GetEntityItem(oldParent) : model->invisibleRootItem();
	int oldRow = model->GetEntityItem(entity)->row();

	QList<QStandardItem*> itemsToMove = oldParentItem->takeRow(oldRow);

	Entity newParent = LevelManager::GetEntityParent(newNext);
	QStandardItem* newParentItem = newParent.IsValid() ? model->GetEntityItem(newParent) : model->invisibleRootItem();
	
	int newRow = model->GetEntityItem(newNext)->row();
	newParentItem->insertRows(newRow, itemsToMove);
}
void EntityHierarchyTreeView::OnEntityMovedAfter(const Entity& entity, const Entity& newPrevious)
{
	Entity oldParent = LevelManager::GetEntityParent(entity);
	QStandardItem* oldParentItem = oldParent.IsValid() ? model->GetEntityItem(oldParent) : model->invisibleRootItem();
	int oldRow = model->GetEntityItem(entity)->row();

	QList<QStandardItem*> itemsToMove = oldParentItem->takeRow(oldRow);

	Entity newParent = LevelManager::GetEntityParent(newPrevious);
	QStandardItem* newParentItem = newParent.IsValid() ? model->GetEntityItem(newParent) : model->invisibleRootItem();
	
	int newRow = model->GetEntityItem(newPrevious)->row() + 1;
	newParentItem->insertRows(newRow, itemsToMove);
}