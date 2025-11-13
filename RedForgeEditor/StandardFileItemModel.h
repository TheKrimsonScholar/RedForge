//#pragma once
//
//#include "LevelManager.h"
//
//#include <QApplication>
//#include <QTreeView>
//#include <QStandardItemModel>
//#include <QString>
//#include <QDebug>
//#include <QIODevice>
//#include <QMimeData>
//#include <QDataStream>
//
//#include "HierarchicalItemModel.h"
//
//Q_DECLARE_METATYPE(Entity);
//
//QDataStream& operator<<(QDataStream& out, const Entity& data);
//QDataStream& operator>>(QDataStream& in, Entity& data);
//
//class StandardFileItemModel : public HierarchicalItemModel
//{
//    Q_OBJECT
//
//private:
//    std::unordered_map<Entity, QStandardItem*> entityItems;
//
//public:
//    explicit StandardFileItemModel(QObject* parent = nullptr);
//    ~StandardFileItemModel();
//
//    void InitializeHierarchy();
//
//protected:
//    //QStringList mimeTypes() const override
//    //{
//    //    return { "application/x-entityitems" };
//    //}
//    //Qt::DropActions supportedDropActions() const override
//    //{
//    //    return Qt::CopyAction | Qt::MoveAction;
//    //}
//    //Qt::ItemFlags flags(const QModelIndex& index) const override
//    //{
//    //    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);
//    //    if (index.isValid())
//    //        return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
//    //    else
//    //        return defaultFlags | Qt::ItemIsDropEnabled; // root accepts drops
//    //}
//    //QMimeData* mimeData(const QModelIndexList& indexes) const override
//    //{
//    //    auto* mimeData = new QMimeData();
//    //    QByteArray encoded;
//    //    QDataStream stream(&encoded, QIODevice::WriteOnly);
//
//    //    // Store data for all dragged indexes
//    //    for (const QModelIndex& index : indexes)
//    //    {
//    //        if (index.isValid()) {
//    //            QString text = data(index, Qt::DisplayRole).toString();
//    //            stream << index.row() << index.column() << text;
//    //        }
//    //    }
//
//    //    mimeData->setData("application/x-entityitems", encoded);
//    //    return mimeData;
//    //}
//    //QMimeData* mimeData(const QModelIndexList& indexes) const
//    //{
//    //    QModelIndexList sorted = indexes;
//
//    //    // Step 2. Filter to only one column (optional, avoids duplicates)
//    //    sorted.erase(std::remove_if(sorted.begin(), sorted.end(),
//    //        [](const QModelIndex& idx) { return idx.column() != 0; }),
//    //        sorted.end());
//
//    //    // Step 3. Sort by absolute position in the model (depth-first)
//    //    std::sort(sorted.begin(), sorted.end(),
//    //        [](const QModelIndex& a, const QModelIndex& b)
//    //        {
//    //            // Walk up to build absolute path from root
//    //            auto path = [](const QModelIndex& idx) {
//    //                QList<int> result;
//    //                for (QModelIndex cur = idx; cur.isValid(); cur = cur.parent())
//    //                    result.prepend(cur.row());
//    //                return result;
//    //                };
//    //            const QList<int> pa = path(a);
//    //            const QList<int> pb = path(b);
//    //            return pa < pb; // QList<int> has lexicographical compare
//    //        });
//
//    //    // Step 4. Use Qt's built-in encoding for standard behavior
//    //    QMimeData* data = QStandardItemModel::mimeData(sorted);
//
//    //    return data;
//
//    //    //QMimeData* data = QStandardItemModel::mimeData(indexes);
//
//    //    //// Absolute row indices
//    //    //QList<QPersistentModelIndex> persistentIndices;
//    //    //for(const QModelIndex& index : indexes)
//    //    //    //if (index.column() == 0)
//    //    //        persistentIndices.append(QPersistentModelIndex(index));
//
//    //    //QByteArray encoded;
//    //    //QDataStream out(&encoded, QIODevice::WriteOnly);
//    //    //out << persistentIndices;
//
//    //    //data->setData("application/x-redforge-hierarchy", encoded);
//    //    //return data;
//    //}
//    //QMimeData* mimeData(const QModelIndexList& indexes) const override
//    //{
//    //    if (indexes.empty())
//    //        return nullptr;
//
//    //    // Copy only the first column (avoid duplicate items)
//    //    QModelIndexList filtered;
//    //    filtered.reserve(indexes.size());
//    //    for (const QModelIndex& idx : indexes)
//    //        if (idx.column() == 0)
//    //            filtered.push_back(idx);
//
//    //    // Sort by absolute "depth-first" visual order in the model
//    //    auto depthFirstKey = [](const QModelIndex& idx) {
//    //        QVector<int> chain;
//    //        for (QModelIndex cur = idx; cur.isValid(); cur = cur.parent())
//    //            chain.prepend(cur.row());
//    //        // Encode chain as a single integer key to preserve hierarchy order
//    //        qint64 key = 0;
//    //        for (int r : chain)
//    //            key = key * 1024 + r; // 1024 safe upper bound for child count
//    //        return key;
//    //        };
//
//    //    std::sort(filtered.begin(), filtered.end(),
//    //        [&](const QModelIndex& a, const QModelIndex& b) {
//    //            return depthFirstKey(a) < depthFirstKey(b);
//    //        });
//
//    //    // Let QStandardItemModel handle the standard MIME encoding
//    //    return QStandardItemModel::mimeData(filtered);
//    //}
//    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
//
//
//
//
//
//
//
//
//    //bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override
//    //{
//    //    // 2. Get the parent item
//    //    QStandardItem* parentItem = itemFromIndex(parent);
//    //    if (!parentItem)
//    //        parentItem = invisibleRootItem();
//
//    //    // 3. Collect all children under parent with their depth-first paths
//    //    std::vector<QStandardItem*> children;
//    //    for (int r = 0; r < parentItem->rowCount(); ++r)
//    //        children.push_back(parentItem->child(r));
//
//    //    auto depthFirstKey = [](QStandardItem* item) {
//    //        std::vector<int> path;
//    //        for (QStandardItem* cur = item; cur->parent(); cur = cur->parent()) {
//    //            path.insert(path.begin(), cur->row()); // row() gives index under parent
//    //        }
//    //        return path;
//    //        };
//
//    //    // 4. Sort children by their original depth-first order
//    //    std::sort(children.begin(), children.end(),
//    //        [&](QStandardItem* a, QStandardItem* b) {
//    //            return depthFirstKey(a) < depthFirstKey(b);
//    //        });
//
//    //    // 5. Re-insert them in correct order
//    //    //for (int i = 0; i < (int)children.size(); ++i)
//    //    //    parentItem->takeRow(children[i]->row()); // remove first
//    //    //for (int i = 0; i < (int)children.size(); ++i)
//    //    //    parentItem->insertRow(i, children[i]); // insert in correct order
//    //    for (int i = 0; i < (int)children.size(); ++i)
//    //    {
//    //        // Remove the item from its current position
//    //        QList<QStandardItem*> rowItems = parentItem->takeRow(children[i]->row());
//
//    //        // Re-insert it at the target index
//    //        parentItem->insertRow(i, rowItems);
//    //    }
//
//    //    /*if (!data->hasFormat("application/x-entityitems"))
//    //        return false;
//
//    //    QByteArray encoded = data->data("application/x-entityitems");
//    //    QDataStream stream(&encoded, QIODevice::ReadOnly);
//
//    //    while (!stream.atEnd()) {
//    //        int srcRow, srcCol;
//    //        QString text;
//    //        stream >> srcRow >> srcCol >> text;
//
//    //        QStandardItem* item = new QStandardItem(text);
//    //        QStandardItem* parentItem = itemFromIndex(parent);
//    //        if (!parentItem)
//    //            parentItem = invisibleRootItem();
//
//    //        if (row < 0)
//    //            parentItem->appendRow(item);
//    //        else
//    //            parentItem->insertRow(row++, item);
//    //    }
//
//    //    return true;*/
//
//    //    // Call base implementation first (actually moves items in the model)
//    //    bool result = QStandardItemModel::dropMimeData(data, action, row, column, parent);
//    //    if(result)
//    //    {
//    //        {
//    //            //QList<QStandardItem*> draggedItems;
//    //            std::unordered_set<Entity> draggedEntities;
//
//    //            QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
//    //            QDataStream stream(&encoded, QIODevice::ReadOnly);
//
//    //            QList<QPair<int, QMap<int, QVariant>>> draggedItems;
//    //            std::vector<Entity> entities;
//
//    //            // Decode all dragged indexes
//    //            while (!stream.atEnd())
//    //            {
//    //                int r, c;
//    //                QMap<int, QVariant> roleData;
//    //                stream >> r >> c >> roleData;
//
//    //                entities.push_back(roleData[Qt::UserRole].value<Entity>());
//    //                //draggedItems.append(qMakePair(r, roleData));
//    //            }
//
//    //            qDebug() << "K";
//    //            std::vector<Entity> entitiesToMove;
//    //            for(const Entity& e : entities)
//    //                if(!LevelManager::IsEntityAncestorContainedInList(e, entitiesToMove))
//    //                    entitiesToMove.push_back(e);
//    //            for(const Entity& e : entitiesToMove)
//    //                qDebug() << "ENTITY " << LevelManager::GetEntityName(e);
//    //            // Now you have all the dragged items
//    //            /*for (const auto& itemData : draggedItems)
//    //            {
//    //                int sourceRow = itemData.first;
//    //                const QMap<int, QVariant>& roles = itemData.second;
//
//    //                QString text = roles.value(Qt::DisplayRole).toString();
//    //                QVariant customValue = roles.value(Qt::UserRole);
//
//    //                qDebug() << "Dragged item row:" << sourceRow << "text:" << text << "custom:" << customValue;
//    //            }*/
//    //        }
//
//    //        //const QString mimeType = "application/x-qabstractitemmodeldatalist";
//
//    //        //if (!data->hasFormat(mimeType)) {
//    //        //    // Let the base class handle external drops if necessary
//    //        //    return QStandardItemModel::dropMimeData(data, action, row, column, parent);
//    //        //}
//
//    //        //QByteArray encodedData = data->data(mimeType);
//    //        //QDataStream stream(&encodedData, QIODevice::ReadOnly);
//
//    //        //int numItems;
//    //        //stream >> numItems; // 1. Read the count of items
//
//    //        //// If numItems is 0 or stream is corrupt, this loop won't run, 
//    //        //// which is a safe way to handle an empty/bad drop.
//
//    //        //for (int i = 0; i < numItems; ++i) {
//    //        //    QModelIndex sourceIndex;
//    //        //    QMap<int, QVariant> roleDataMap;
//
//    //        //    // 2. Read the QModelIndex object
//    //        //    // This internally reads the row, column, and parent information required to reconstruct the index.
//    //        //    stream >> sourceIndex;
//
//    //        //    // 3. Read the role data map
//    //        //    stream >> roleDataMap;
//
//    //        //    // --- Your Application Logic ---
//    //        //    if(roleDataMap.contains(Qt::UserRole))
//    //        //    {
//    //        //        Entity e = roleDataMap[Qt::UserRole].value<Entity>();
//
//    //        //        qDebug() << "MOVE " << LevelManager::GetEntityName(e);
//
//    //        //        if(!draggedEntities.contains(LevelManager::GetEntityParent(e)))
//    //        //            draggedEntities.emplace(e);
//    //        //    }
//    //        //}
//
//    //        //{
//    //        //    const QString mimeType = "application/x-qabstractitemmodeldatalist";
//
//    //        //    if (!data->hasFormat(mimeType)) {
//    //        //        // Let the base class handle external drops if necessary
//    //        //        return QStandardItemModel::dropMimeData(data, action, row, column, parent);
//    //        //    }
//
//    //        //    // 1. Setup QDataStream (using the MIME type that worked for you)
//    //        //    QByteArray encodedData = data->data(mimeType);
//    //        //    QDataStream stream(&encodedData, QIODevice::ReadOnly);
//
//    //        //    int r_skip, c_skip;
//    //        //    QMap<int, QVariant> map_skip;
//    //        //    stream >> r_skip >> c_skip >> map_skip;
//    //        //    // The stream pointer is now at the start of the first actual dragged item.
//
//    //        //    // 2. Loop for the actual number of items dragged (2 in this case)
//    //        //    // Replace '2' with a dynamic count if you know the selection size.
//    //        //    int numDraggedItems = 2;
//
//    //        //    for (int i = 0; i < numDraggedItems; ++i) {
//    //        //        int r, c;
//    //        //        QMap<int, QVariant> roleDataMap;
//
//    //        //        // Read the actual item's data
//    //        //        stream >> r >> c >> roleDataMap;
//
//    //        //        // --- Process the item ---
//    //        //        // ...
//    //        //        qDebug() << "Successfully Decoded Item" << i << "Row:" << r;
//    //        //    }
//    //        //}
//
//    //        //QByteArray encoded = data->data("application/x-qstandarditemmodeldatalist");
//    //        //QDataStream stream(&encoded, QIODevice::ReadOnly);
//    //        //while(!stream.atEnd())
//    //        //{
//    //        //    int r, c;
//    //        //    QMap<int, QVariant> roleDataMap;
//    //        //    stream >> r >> c >> roleDataMap;
//
//    //        //    Entity e = roleDataMap[Qt::UserRole].value<Entity>();
//    //        //    if(roleDataMap[Qt::UserRole].isValid())
//    //        //        qDebug() << "MOVE " << LevelManager::GetEntityName(e);
//    //        //    // If we're moving the parent as well, don't bother adding this entity; it will automatically be brought with the parent
//    //        //    // Note that we can assume the parent is added first, since the hierarchy ensures parents come before their children
//    //        //    if(!draggedEntities.contains(LevelManager::GetEntityParent(e)))
//    //        //        draggedEntities.emplace(e);
//    //        //}
//
//    //        /*{
//    //            Entity entity;
//
//    //            QStandardItem* parentItem = itemFromIndex(parent);
//    //            Entity parent = parentItem->data(Qt::UserRole).value<Entity>();
//    //            if(row == -1)
//    //            {
//    //                std::cout << "REPARENT " << LevelManager::GetEntityName(entity) << " TO " << LevelManager::GetEntityName(parent) << std::endl;
//    //            }
//    //            else
//    //            {
//    //                QStandardItem* itemBefore = parentItem->child(row - 1);
//    //                QStandardItem* itemAfter = parentItem->child(row + 1);
//    //            
//    //                Entity sibling = itemBefore->data(Qt::UserRole).value<Entity>();
//    //                Entity siblingAfter = itemAfter->data(Qt::UserRole).value<Entity>();
//    //            
//    //                std::cout << "ROW: " << row
//    //                    << " PARENT: " << LevelManager::GetEntityName(parent)
//    //                    << " SIBLING BEFORE: " << LevelManager::GetEntityName(sibling)
//    //                    << " SIBLING AFTER: " << LevelManager::GetEntityName(siblingAfter) << std::endl;
//    //            }
//
//    //        }*/
//
//    //        /* Determine target item */
//
//    //        /*if(row == -1 && column == -1)
//    //        {
//    //            std::cout << "REPARENT" << std::endl;
//    //        }
//    //        else if(row >= 0)
//    //        {
//    //            std::cout << "REORDER" << std::endl;
//    //        }
//    //        itemFromIndex(parent);
//    //        QStandardItem* itemBefore = nullptr;
//    //        if(row > 0)
//    //            itemBefore = itemFromIndex(parent.sibling(row - 1, column));
//
//    //        QStandardItem* itemAfter = nullptr;
//    //        QModelIndex indexAfter = parent.sibling(row, column);
//    //        if(indexAfter.isValid())
//    //            itemAfter = itemFromIndex(indexAfter);
//
//    //        Entity e = { 60, 90 };
//    //        Entity entityBefore = (itemBefore ? itemBefore->data(Qt::UserRole).value<Entity>() : e);
//    //        Entity entityAfter = (itemAfter ? itemAfter->data(Qt::UserRole).value<Entity>() : e);
//    //        qDebug() << "BEFORE " << std::format("{}", entityBefore);
//    //        qDebug() << "AFTER " << std::format("{}", entityAfter);*/
//
//    //        //QStandardItem* itemBefore = remove;
//
//    //        //QStandardItem* targetParent = parent.isValid() ? itemFromIndex(parent) : nullptr;
//    //        //int targetRow = row;
//
//    //        //// If row == -1, it's appended
//    //        //if (targetRow == -1) {
//    //        //    targetRow = targetParent ? targetParent->rowCount() : rowCount();
//    //        //}
//
//    //        //QStandardItem* targetItem = nullptr;
//    //        //if (targetParent)
//    //        //    targetItem = targetParent->child(targetRow);
//    //        //else if (targetRow < rowCount())
//    //        //    targetItem = item(targetRow);
//
//    //        //qDebug() << "PARENT " << (targetParent ? targetParent->data(Qt::UserRole) : "NULL");
//    //        //qDebug() << "TARGET " << (targetItem ? targetItem->data(Qt::UserRole) : "NULL");
//
//    //        /*if (draggedItems[0]->parent() != targetParent)
//    //        {
//    //            qDebug() << "REPARENT " << (targetParent ? targetParent->data(Qt::UserRole) : "NULL");
//    //        }
//    //        else
//    //        {
//    //            qDebug() << "REORDER" << (targetParent ? targetParent->data(Qt::UserRole) : "NULL");
//    //        }*/
//    //    }
//
//    //    return result;
//    //}
//
//    void OnItemsReparented(std::vector<ItemInfo> items, QStandardItem* parentItem) override;
//    void OnItemsReorderedBefore(std::vector<ItemInfo> items, QStandardItem* nextSiblingItem) override;
//    void OnItemsReorderedAfter(std::vector<ItemInfo> items, QStandardItem* previousSiblingItem) override;
//
//    void OnEntityCreated(const Entity& entity);
//    void OnEntityDestroyed(const Entity& entity);
//};