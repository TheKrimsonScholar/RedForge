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
    //QFrame* frame;
    QFrame* content;

    std::vector<ComponentVariableEntry*> variableEntries;

    Entity entity;
    std::type_index componentTypeID;

    bool isExpanded = false;

public:
    ComponentEntry(const Entity& entity, std::type_index componentTypeID, void* componentPtr, QWidget* parent = nullptr);
    ~ComponentEntry();

protected:
    void AppendVariableWidget(const ComponentVariableInfo& variableInfo, void* variablePtr);

    void UpdateDisplayedVariables();
};