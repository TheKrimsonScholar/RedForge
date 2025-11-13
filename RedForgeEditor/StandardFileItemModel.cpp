//#include "StandardFileItemModel.h"
//
//QDataStream& operator<<(QDataStream& out, const Entity& data)
//{
//	return out << data.index << data.generation;
//}
//QDataStream& operator>>(QDataStream& in, Entity& data)
//{
//	return in >> data.index >> data.generation;
//}
//
//void registerMyCustomTypes()
//{
//	qRegisterMetaType<Entity>("Entity");
//}
//
//StandardFileItemModel::StandardFileItemModel(QObject* parent) : HierarchicalItemModel(parent)
//{
//    setHorizontalHeaderLabels({ "Name", "Level" });
//
//    //// Example items
//    //QStandardItem* item2 = new QStandardItem("Item 2");
//    //item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
//    //item2->setData("Entity 2", Qt::UserRole);
//
//    //appendRow(item2);
//}
//StandardFileItemModel::~StandardFileItemModel()
//{
//
//}
//
//void StandardFileItemModel::InitializeHierarchy()
//{
//    clear();
//
//    // Gather any existing entities in the level
//    LevelManager::ForEachEntity(
//        [this](const Entity& entity)
//        {
//            std::cout << LevelManager::GetEntityName(entity) << " " << entity.index << " " << entity.generation << std::endl;
//
//            QStandardItem* item = new QStandardItem(LevelManager::GetEntityName(entity).c_str());
//            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
//            //item->setData(LevelManager::GetEntityName(entity).c_str(), Qt::UserRole);
//            item->setData(QVariant::fromValue(entity), Qt::UserRole);
//
//            entityItems.emplace(entity, item);
//
//            Entity parent = LevelManager::GetEntityParent(entity);
//            if(parent.IsValid())
//                entityItems.at(parent)->appendRow(item);
//            else
//                appendRow(item);
//        });
//
//    // Bind events for when entities are created and destroyed
//    LevelManager::GetOnEntityCreated()->AddUnique(EventCallback(this, &StandardFileItemModel::OnEntityCreated));
//    LevelManager::GetOnEntityDestroyed()->AddUnique(EventCallback(this, &StandardFileItemModel::OnEntityDestroyed));
//}
//
//bool StandardFileItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
//{
//    // Let base handle the move first
//    bool result = HierarchicalItemModel::dropMimeData(data, action, row, column, parent);
//    if(!result)
//        return false;
//
//    // After base move, reorder children under 'parent' to match absolute index
//    //QStandardItem* parentItem = itemFromIndex(parent);
//    //if (!parentItem)
//    //    parentItem = invisibleRootItem();
//
//    //// Collect all children
//    //std::vector<QStandardItem*> children;
//    //for (int i = 0; i < parentItem->rowCount(); ++i)
//    //    children.push_back(parentItem->child(i));
//
//    //// Sort by depth-first path stored in Qt::UserRole (from mimeData)
//    //std::sort(children.begin(), children.end(),
//    //    [](QStandardItem* a, QStandardItem* b) {
//    //        return a->data(Qt::UserRole + 1).toLongLong() <
//    //            b->data(Qt::UserRole + 1).toLongLong();
//    //    });
//
//    //// Re-insert in sorted order
//    //for (int i = 0; i < (int)children.size(); ++i)
//    //{
//    //    QList<QStandardItem*> rowItems = parentItem->takeRow(children[i]->row());
//    //    parentItem->insertRow(i, rowItems);
//    //}
//
//    return true;
//}
//
//void StandardFileItemModel::OnItemsReparented(std::vector<ItemInfo> items, QStandardItem* parentItem)
//{
//    Entity parent = parentItem->data(Qt::UserRole).value<Entity>();
//    qDebug() << "REPARENT ITEMS " << LevelManager::GetEntityName(parent);
//    for(const ItemInfo& item : items)
//    {
//        Entity entity = item.item->data(Qt::UserRole).value<Entity>();
//        qDebug() << LevelManager::GetEntityName(entity);
//
//        LevelManager::SetEntityParent(entity, parent);
//    }
//}
//void StandardFileItemModel::OnItemsReorderedBefore(std::vector<ItemInfo> items, QStandardItem* nextSiblingItem)
//{
//    Entity entityAfter = nextSiblingItem->data(Qt::UserRole).value<Entity>();
//    qDebug() << "REORDER ITEMS BEFORE " << LevelManager::GetEntityName(entityAfter);
//    for(const ItemInfo& item : items)
//    {
//        Entity entity = item.item->data(Qt::UserRole).value<Entity>();
//        qDebug() << LevelManager::GetEntityName(entity);
//
//        LevelManager::MoveEntityBefore(entity, entityAfter);
//    }
//}
//void StandardFileItemModel::OnItemsReorderedAfter(std::vector<ItemInfo> items, QStandardItem* previousSiblingItem)
//{
//    Entity entityBefore = previousSiblingItem->data(Qt::UserRole).value<Entity>();
//    qDebug() << "REORDER ITEMS AFTER " << LevelManager::GetEntityName(entityBefore);
//    // Iterate through items in reverse order so that final order is correct
//    for(int i = items.size() - 1; i >= 0; i--)
//    {
//        Entity entity = items[i].item->data(Qt::UserRole).value<Entity>();
//        qDebug() << LevelManager::GetEntityName(entity);
//
//        LevelManager::MoveEntityAfter(entity, entityBefore);
//    }
//
//    LevelManager::ForEachEntity(
//        [](const Entity& entity)
//        {
//            qDebug() << LevelManager::GetEntityName(entity);
//        });
//}
//
//void StandardFileItemModel::OnEntityCreated(const Entity& entity)
//{
//    QStandardItem* item = new QStandardItem("Item 1");
//    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
//    item->setData(LevelManager::GetEntityName(entity).c_str(), Qt::UserRole);
//
//    entityItems.emplace(entity, item);
//
//    Entity parent = LevelManager::GetEntityParent(entity);
//    if(parent.IsValid())
//        entityItems.at(parent)->appendRow(item);
//    else
//        appendRow(item);
//}
//void StandardFileItemModel::OnEntityDestroyed(const Entity& entity)
//{
//
//}