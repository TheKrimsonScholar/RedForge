#pragma once

#include "EntityManager.h"

#include <QGroupBox>
#include <QToolButton>
#include <QLabel>
#include <QFrame>

#include "ComponentVariableEntry.h"

class ComponentEntry : public QWidget
{
    Q_OBJECT

private:
    QToolButton* expandableArrow;
    QLabel* label;
    QToolButton* removeButton;
    QFrame* content;

    std::vector<ComponentVariableEntry*> variableEntries;

    Entity entity;
    TypeID componentTypeID;

    bool isExpanded = false;

public:
    ComponentEntry(const Entity& entity, TypeID componentTypeID, void* componentPtr, QWidget* parent = nullptr);
    ~ComponentEntry();

protected:
    void AppendVariableWidget(const ComponentVariableTypeInfo& variableInfo, void* variablePtr);

    void UpdateDisplayedVariables();

    void OnVariableChanged(void* variablePtr);
};