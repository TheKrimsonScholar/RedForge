#pragma once

#include <QStandardItemModel>

#include "LevelManager.h"

#include <QApplication>
#include <QTreeView>
#include <QString>
#include <QDebug>
#include <QIODevice>
#include <QMimeData>
#include <QDataStream>

Q_DECLARE_METATYPE(Entity);

QDataStream& operator<<(QDataStream& out, const Entity& data);
QDataStream& operator>>(QDataStream& in, Entity& data);

class EntityHierarchyItemModel : public QStandardItemModel
{
    Q_OBJECT

private:
    std::unordered_map<Entity, QStandardItem*> entityItems;

public:
    explicit EntityHierarchyItemModel(QObject* parent = nullptr);
    ~EntityHierarchyItemModel();

    void InitializeHierarchy();

protected:
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

    void CreateEntityItem(const Entity& entity);
    void DestroyEntityItem(const Entity& entity);

    int GetAbsoluteHierarchyIndex(const Entity& entity) const;

    friend class EntityHierarchyTreeView;

public:
    QStandardItem* GetEntityItem(const Entity& entity) const { return entityItems.find(entity) != entityItems.end() ? entityItems.at(entity) : nullptr; }
};