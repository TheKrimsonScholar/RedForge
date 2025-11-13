#include "HierarchicalItemModel.h"

HierarchicalItemModel::HierarchicalItemModel(QObject* parent) : QStandardItemModel(parent)
{

}
HierarchicalItemModel::~HierarchicalItemModel()
{

}

bool HierarchicalItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    if(action != Qt::MoveAction) return false;

    // find the QTreeView using this model
    QTreeView* view = nullptr;
    for(QObject* w : qApp->allWidgets())
        if(auto tv = qobject_cast<QTreeView*>(w))
            if(tv->model() == this)
            {
                view = tv;
                break;
            }
    if(!view)
        return false;

    // get selection (may include many indexes per row; we only need unique items)
    auto selectedIndexes = view->selectionModel()->selectedIndexes();
    if(selectedIndexes.isEmpty())
        return false;

    // 1) build absolute pre-order list and map item->absIndex (before removals)
    std::vector<QStandardItem*> preorder;
    BuildPreorderList(invisibleRootItem(), preorder);

    // map from item pointer to its absolute index for quick lookup
    std::unordered_map<QStandardItem*, int> absIndexMap;
    absIndexMap.reserve(preorder.size());
    for(size_t i = 0; i < preorder.size(); ++i)
        absIndexMap[preorder[i]] = static_cast<int>(i);

    // 2) collect unique top-level selected items (skip children of selected parents)
    QSet<QStandardItem*> selSet;
    for(const QModelIndex& idx : selectedIndexes)
        selSet.insert(itemFromIndex(idx));

    std::vector<ItemInfo> itemsToMove;
    itemsToMove.reserve(selSet.size());

    for(QStandardItem* it : selSet)
    {
        // skip if ancestor is also selected
        QStandardItem* p = it->parent();
        bool skip = false;
        while(p)
        {
            if(selSet.contains(p))
            {
                skip = true;
                break;
            }
            p = p->parent();
        }
        if(skip)
            continue;
        
        int abs = -1;
        auto itmap = absIndexMap.find(it);
        if(itmap != absIndexMap.end()) 
            abs = itmap->second;
        
        itemsToMove.emplace_back(it, it->parent() ? it->parent() : invisibleRootItem(), it->row(), abs);
    }

    if(itemsToMove.empty())
        return false;

    // 3) compute drop absolute index (position in preorder) BEFORE removals
    //    If parent is invalid => root, else newParent is parent
    QStandardItem* newParent = parent.isValid() ? itemFromIndex(parent) : invisibleRootItem();
    QStandardItem* newNextSibling = newParent->child(row);
    QStandardItem* newPreviousSibling = newParent->child(row - 1);

    int dropAbsIndex = -1;
    if(row == -1)
    {
        // drop on parent -> place after all descendants of parent
        // parent's absolute index:
        if(newParent == invisibleRootItem())
            // append to end of whole list
            dropAbsIndex = static_cast<int>(preorder.size());
        else
        {
            auto itmap = absIndexMap.find(newParent);
            if(itmap == absIndexMap.end())
                // parent not found in preorder (shouldn't happen), append to end
                dropAbsIndex = static_cast<int>(preorder.size());
            else
            {
                int parentAbs = itmap->second;
                // insertion after all nodes in parent's subtree
                int subSize = GetSubtreeSize(newParent);
                dropAbsIndex = parentAbs + subSize;
            }
        }
    }
    else
    {
        // drop between siblings: the child at 'row' under newParent corresponds to a preorder entry
        if(newParent == invisibleRootItem())
        {
            // child is top-level at index 'row'
            if(row < newParent->rowCount())
            {
                QStandardItem* child = newParent->child(row);
                auto itmap = absIndexMap.find(child);
                if(itmap != absIndexMap.end())
                    dropAbsIndex = itmap->second;
            }
            else
                dropAbsIndex = static_cast<int>(preorder.size());
        }
        else
        {
            if(row < newParent->rowCount())
            {
                QStandardItem* child = newParent->child(row);
                auto itmap = absIndexMap.find(child);
                if(itmap != absIndexMap.end())
                    dropAbsIndex = itmap->second;
                else
                    dropAbsIndex = static_cast<int>(preorder.size());
            }
            else
            {
                // row == newParent->rowCount(): position after parent's subtree
                auto itmap = absIndexMap.find(newParent);
                if(itmap != absIndexMap.end())
                    dropAbsIndex = itmap->second + GetSubtreeSize(newParent);
                else
                    dropAbsIndex = static_cast<int>(preorder.size());
            }
        }
    }

    if(dropAbsIndex < 0)
        dropAbsIndex = static_cast<int>(preorder.size());

    // 4) sort itemsToMove by absIndex ascending to preserve visual order on insertion
    std::sort(itemsToMove.begin(), itemsToMove.end(),
        [](const ItemInfo& a, const ItemInfo& b) { return a.oldAbsoluteIndex < b.oldAbsoluteIndex; });

    // Save expanded states before moving
    QSet<QStandardItem*> expanded;
    SaveExpandedItems(view, invisibleRootItem(), expanded);

    // 5) remove items from model in descending absolute index order (to avoid shifting earlier nodes)
    std::sort(itemsToMove.begin(), itemsToMove.end(),
        [](const ItemInfo& a, const ItemInfo& b) { return a.oldAbsoluteIndex > b.oldAbsoluteIndex; });
    for(const ItemInfo& info : itemsToMove)
    {
        // care: oldRow is row at time BEFORE removals; takeRow uses that
        if(info.oldParent)
            info.oldParent->takeRow(info.oldRow);
        else
            invisibleRootItem()->takeRow(info.oldRow);
    }

    // 6) rebuild preorder list and map after removals
    std::vector<QStandardItem*> preorderAfter;
    BuildPreorderList(invisibleRootItem(), preorderAfter);
    std::unordered_map<QStandardItem*, int> absAfter;
    absAfter.reserve(preorderAfter.size());
    for(size_t i = 0; i < preorderAfter.size(); ++i)
        absAfter[preorderAfter[i]] = static_cast<int>(i);

    // IMPORTANT: dropAbsIndex was computed in the original space.
    // We need to map that absolute index to an insertion point in the updated tree:
    // Find the first item in preorderAfter whose original absolute index >= dropAbsIndex.
    // To do that we can locate insertion point by counting remaining nodes in newParent whose
    // original absolute index is < dropAbsIndex. This avoids trying to map dropAbsIndex across removals.
    //
    // Build a set of moved items for quick test
    QSet<QStandardItem*> movedSet;
    for(const ItemInfo& it : itemsToMove)
        movedSet.insert(it.item);

    // 7) compute insertRow inside newParent by counting children remaining whose original abs < dropAbsIndex
    int insertRow = 0;
    if(newParent == invisibleRootItem())
    {
        // top-level: count top-level children with original abs < dropAbsIndex and not moved
        for(int i = 0; i < newParent->rowCount(); i++)
        {
            QStandardItem* child = newParent->child(i);
            // if child was part of movedSet, skip
            if(movedSet.contains(child))
                continue;
            
            // original abs index:
            auto itOld = absIndexMap.find(child);
            int childOldAbs = (itOld != absIndexMap.end()) ? itOld->second : INT_MAX;
            if(childOldAbs < dropAbsIndex)
                ++insertRow;
        }
    }
    else
    {
        // for children of newParent, same logic
        for(int i = 0; i < newParent->rowCount(); i++)
        {
            QStandardItem* child = newParent->child(i);
            if(movedSet.contains(child))
                continue;
            
            auto itOld = absIndexMap.find(child);
            int childOldAbs = (itOld != absIndexMap.end()) ? itOld->second : INT_MAX;
            if(childOldAbs < dropAbsIndex)
                ++insertRow;
        }
    }

    // 8) finally, insert the moved items in ascending original absolute order
    //    (we earlier sorted ascending, now re-sort since we removed in descending)
    std::sort(itemsToMove.begin(), itemsToMove.end(),
        [](const ItemInfo& a, const ItemInfo& b) { return a.oldAbsoluteIndex < b.oldAbsoluteIndex; });

    for(const ItemInfo& info : itemsToMove)
    {
        newParent->insertRow(insertRow, info.item);
        ++insertRow;
    }

    // Restore expanded states after move
    RestoreExpandedItems(view, invisibleRootItem(), expanded);

    // After moving the rows, call custom logic based on whether it was a reparent operation or a reorder operation
    // Reparent
    if(row == -1)
        OnItemsReparented(itemsToMove, newParent);
    // Reorder
    else
        // Move before (only necessary if set as first child)
        if(row == 0)
            OnItemsReorderedBefore(itemsToMove, newNextSibling);
        // Move after
        else
            OnItemsReorderedAfter(itemsToMove, newPreviousSibling);

    return true;
}

void HierarchicalItemModel::BuildPreorderList(QStandardItem* root, std::vector<QStandardItem*>& out)
{
    for(int i = 0; i < root->rowCount(); ++i)
    {
        QStandardItem* child = root->child(i);
        // pre-order: visit node, then children
        std::function<void(QStandardItem*)> visit = [&](QStandardItem* node)
            {
                out.push_back(node);
                for(int c = 0; c < node->rowCount(); c++)
                    visit(node->child(c));
            };
        visit(child);
    }
}
int HierarchicalItemModel::GetSubtreeSize(QStandardItem* node)
{
    if(!node)
        return 0;
        
    int cnt = 1;
    for(int i = 0; i < node->rowCount(); i++)
        cnt += GetSubtreeSize(node->child(i));
        
    return cnt;
}

void HierarchicalItemModel::SaveExpandedItems(QTreeView* view, QStandardItem* item, QSet<QStandardItem*>& inout_expanded)
{
    if(!item)
        return;

    for(int i = 0; i < item->rowCount(); i++)
    {
        QStandardItem* child = item->child(i);
        if(!child)
            continue;

        QModelIndex index = view->model()->index(i, 0, item->index());
        if(view->isExpanded(index))
            inout_expanded.insert(child);

        SaveExpandedItems(view, child, inout_expanded);
    }
}
void HierarchicalItemModel::RestoreExpandedItems(QTreeView* view, QStandardItem* item, const QSet<QStandardItem*>& expanded)
{
    if(!item)
        return;

    for(int i = 0; i < item->rowCount(); i++)
    {
        QStandardItem* child = item->child(i);
        if(!child)
            continue;

        QModelIndex index = view->model()->index(i, 0, item->index());
        if(expanded.contains(child))
            view->setExpanded(index, true);

        RestoreExpandedItems(view, child, expanded);
    }
}