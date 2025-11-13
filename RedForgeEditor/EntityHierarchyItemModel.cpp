#include "EntityHierarchyItemModel.h"

QDataStream& operator<<(QDataStream& out, const Entity& data)
{
    return out << data.index << data.generation;
}
QDataStream& operator>>(QDataStream& in, Entity& data)
{
    return in >> data.index >> data.generation;
}

EntityHierarchyItemModel::EntityHierarchyItemModel(QObject* parent) : QStandardItemModel(parent)
{
    setHorizontalHeaderLabels({ "Name", "Level" });
}
EntityHierarchyItemModel::~EntityHierarchyItemModel()
{

}

void EntityHierarchyItemModel::InitializeHierarchy()
{
    clear();

    // Gather any existing entities in the level
    LevelManager::ForEachEntity(
        [this](const Entity& entity)
        {
            CreateEntityItem(entity);
        });
}

bool EntityHierarchyItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    std::vector<Entity> draggedEntities;
    QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    while(!stream.atEnd())
    {
        int r, c;
        QMap<int, QVariant> roleDataMap;
        stream >> r >> c >> roleDataMap;
                
        if(roleDataMap[Qt::UserRole].isValid())
        {
            Entity entity = roleDataMap[Qt::UserRole].value<Entity>();

            draggedEntities.push_back(entity);
        }
    }

    std::sort(draggedEntities.begin(), draggedEntities.end(), 
        [this](const Entity& a, const Entity& b) -> bool { return GetAbsoluteHierarchyIndex(a) < GetAbsoluteHierarchyIndex(b); });

    QStandardItem* newParentItem = parent.isValid() ? itemFromIndex(parent) : invisibleRootItem();
    QStandardItem* newNextSiblingItem = newParentItem->child(row);
    QStandardItem* newPreviousSiblingItem = newParentItem->child(row - 1);

    Entity newParent = parent.isValid() ? itemFromIndex(parent)->data(Qt::UserRole).value<Entity>() : Entity();
    Entity newNextSibling = newNextSiblingItem ? newNextSiblingItem->data(Qt::UserRole).value<Entity>() : Entity();
    Entity newPreviousSibling = newPreviousSiblingItem ? newPreviousSiblingItem->data(Qt::UserRole).value<Entity>() : Entity();

    // Call LevelManager logic based on whether it was a reparent operation or a reorder operation (view automatically updated via event bindings)
    // Reparent
    if(row == -1)
        for(const Entity& entity : draggedEntities)
            LevelManager::SetEntityParent(entity, newParent);
    // Reorder
    else
        // Move before (only necessary if set as first child)
        if(row == 0)
            for(const Entity& entity : draggedEntities)
                LevelManager::MoveEntityBefore(entity, newNextSibling);
        // Move after
        else
            // Reverse order when placing multiple entities to the position just after a sibling
            for(int i = draggedEntities.size() - 1; i >= 0; i--)
                LevelManager::MoveEntityAfter(draggedEntities[i], newPreviousSibling);

    return false;
}

void EntityHierarchyItemModel::CreateEntityItem(const Entity& entity)
{
    QStandardItem* item = new QStandardItem(LevelManager::GetEntityName(entity).c_str());
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
    item->setData(QVariant::fromValue(entity), Qt::UserRole);

    entityItems.emplace(entity, item);

    Entity parent = LevelManager::GetEntityParent(entity);
    if(parent.IsValid())
        entityItems.at(parent)->appendRow(item);
    else
        appendRow(item);
}
void EntityHierarchyItemModel::DestroyEntityItem(const Entity& entity)
{
    if(QStandardItem* entityItem = GetEntityItem(entity))
    {
        QStandardItem* parent = entityItem->parent();
	    int rowIndex = entityItem->row();
	    // Remove the item from its parent (or the root if it has no parent)
	    if(parent)
		    parent->removeRow(rowIndex);
	    else
		    removeRow(rowIndex);

        entityItems.erase(entity);
    }
}

int EntityHierarchyItemModel::GetAbsoluteHierarchyIndex(const Entity& entity) const
{
	int absoluteIndex = -1;
    bool entityFound = false;
	LevelManager::ForEachEntity(
		[entity, &absoluteIndex, &entityFound](const Entity& e)
		{
            // Stop incrementing absolute index if we've already found the specified entity
            if(entityFound)
                return;

			absoluteIndex++;

            // If we've reached the specified entity, iteration should stop
            if(e == entity)
                entityFound = true;
		});

	return absoluteIndex;
}