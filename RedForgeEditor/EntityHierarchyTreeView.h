#pragma once

#include <QTreeView>

#include "EntityHierarchyItemModel.h"

class EntityHierarchyTreeView : public QTreeView
{
	Q_OBJECT

private:
	EntityHierarchyItemModel* model;

	std::vector<Entity> selectedEntities;

public:
	explicit EntityHierarchyTreeView(QWidget* parent = nullptr);
	~EntityHierarchyTreeView();

	void InitializeHierarchy();

protected:
	void keyPressEvent(QKeyEvent* event) override;

	void OnEntityCreated(const Entity& entity);
	void OnEntityDestroyed(const Entity& entity);

	void OnEntityReparented(const Entity& entity, const Entity& newParent);
	void OnEntityMovedBefore(const Entity& entity, const Entity& newNext);
	void OnEntityMovedAfter(const Entity& entity, const Entity& newPrevious);

public:
	std::vector<Entity> GetSelectedEntities() const { return selectedEntities; }
};