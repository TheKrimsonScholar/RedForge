#include "HierarchyPanel.h"

#include <QVBoxLayout>

#include "MainEditorWindow.h"

#include "EditorPaths.h"

HierarchyPanel::HierarchyPanel(QWidget* parent) : EditorPanel("Hierarchy", parent)
{
    newEntityButton = new QPushButton(QICON_FROM_PATH("Basic/Plus"), "New Entity", this);
    QObject::connect(newEntityButton, &QPushButton::clicked, this, 
        [this]()
        {
            const Entity& newEntity = LevelManager::CreateEntity();
            
            if(!treeView->GetSelectedEntities().empty())
                LevelManager::SetEntityParent(newEntity, treeView->GetSelectedEntities()[0]);
        });

    treeView = new EntityHierarchyTreeView(this);
    treeView->expandAll();

    QVBoxLayout* vBox = new QVBoxLayout();
    vBox->addWidget(newEntityButton);
    vBox->addWidget(treeView);

    contentArea->setLayout(vBox);
}
HierarchyPanel::~HierarchyPanel()
{

}

void HierarchyPanel::Initialize()
{
    treeView->InitializeHierarchy();
}
void HierarchyPanel::Update()
{

}