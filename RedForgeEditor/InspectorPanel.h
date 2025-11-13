#pragma once

#include "EditorPanel.h"

#include "LevelManager.h"

#include <QLabel>
#include <QVBoxLayout>

class InspectorPanel : public EditorPanel
{
    Q_OBJECT

private:
    QLabel* label;
    QVBoxLayout* componentEntriesBox;

public:
    InspectorPanel(QWidget* parent = nullptr);
    ~InspectorPanel();

protected:
    void Initialize() override;
    void Update() override;

    friend class MainEditorWindow;
    friend class HierarchyPanel;

public:
    void SetTarget(const Entity& entity);

    void ResetTarget();
};