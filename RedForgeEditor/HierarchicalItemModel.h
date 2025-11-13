#pragma once

#include <vector>
#include <functional>

#include <QApplication>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSet>

class HierarchicalItemModel : public QStandardItemModel
{
protected:
    struct ItemInfo
    {
        QStandardItem* item;
        QStandardItem* oldParent;
        int oldRow;
        int oldAbsoluteIndex;
    };

public:
    explicit HierarchicalItemModel(QObject* parent = nullptr);
    ~HierarchicalItemModel();

protected:
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
    
    Qt::DropActions supportedDropActions() const override { return Qt::CopyAction | Qt::MoveAction; }

    virtual void OnItemsReparented(std::vector<ItemInfo> items, QStandardItem* parentItem) = 0;
    virtual void OnItemsReorderedBefore(std::vector<ItemInfo> items, QStandardItem* nextSiblingItem) = 0;
    virtual void OnItemsReorderedAfter(std::vector<ItemInfo> items, QStandardItem* previousSiblingItem) = 0;

private:
    // Build a pre-order list of all QStandardItem* under root (top-level children)
    void BuildPreorderList(QStandardItem* root, std::vector<QStandardItem*>& out);
    // Count size of subtree including node (node + all descendants)
    int GetSubtreeSize(QStandardItem* node);

    void SaveExpandedItems(QTreeView* view, QStandardItem* item, QSet<QStandardItem*>& inout_expanded);
    void RestoreExpandedItems(QTreeView* view, QStandardItem* item, const QSet<QStandardItem*>& expanded);
};