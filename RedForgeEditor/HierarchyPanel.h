#pragma once

#include "EditorPanel.h"

#include <QPushButton>
#include <QTreeView>

#include "EntityHierarchyTreeView.h"

class HierarchyPanel : public EditorPanel
{
    Q_OBJECT

private:
    QPushButton* newEntityButton;
    EntityHierarchyTreeView* treeView;

public:
    HierarchyPanel(QWidget* parent = nullptr);
    ~HierarchyPanel();

protected:
    void Initialize() override;
    void Update() override;

    friend class MainEditorWindow;
};