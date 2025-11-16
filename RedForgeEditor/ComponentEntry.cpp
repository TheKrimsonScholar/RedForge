#include "ComponentEntry.h"

#include "FileManager.h"

#include <QVBoxLayout>

#include "ComponentVariableEntry_Float.h"
#include "ComponentVariableEntry_Vector2.h"
#include "ComponentVariableEntry_Vector3.h"
#include "ComponentVariableEntry_QuaternionEuler.h"
#include "ComponentVariableEntry_Mesh.h"
#include "ComponentVariableEntry_Material.h"

#include "EditorPaths.h"

#include "MainEditorWindow.h"

ComponentEntry::ComponentEntry(const Entity& entity, std::type_index componentTypeID, void* componentPtr, QWidget* parent) : QWidget(parent),
    entity(entity), componentTypeID(componentTypeID)
{
    QVBoxLayout* vBox = new QVBoxLayout();
    setLayout(vBox);

    QHBoxLayout* headerBox = new QHBoxLayout();
    vBox->addLayout(headerBox);

    expandableArrow = new QToolButton(this);
    expandableArrow->setIcon(QICON_FROM_PATH("Basic/Right_Small"));
    expandableArrow->setAutoRaise(true);
    QObject::connect(expandableArrow, &QToolButton::clicked, this, 
        [this]()
        {
            isExpanded = !isExpanded;
            expandableArrow->setIcon(isExpanded ? QICON_FROM_PATH("Basic/Down_Small") : QICON_FROM_PATH("Basic/Right_Small"));
            content->setVisible(isExpanded);
        });

    label = new QLabel((GET_COMPONENT_NAME(componentTypeID)).c_str(), this);

    removeButton = new QToolButton(this);
    removeButton->setIcon(QICON_FROM_PATH("Basic/Cross"));
    removeButton->setMaximumSize(QSize(32, 32));
    QObject::connect(removeButton, &QPushButton::clicked, this, 
        [entity, componentTypeID]()
        {
            EntityManager::RemoveComponentOfType(entity, componentTypeID);

            // Refresh the inspector
            if(MainEditorWindow::Get())
                if(MainEditorWindow::Get()->GetInspectorPanel())
                    MainEditorWindow::Get()->GetInspectorPanel()->SetTarget(entity);
        });

    headerBox->addWidget(expandableArrow);
    headerBox->addWidget(label);
    headerBox->addWidget(removeButton);

    content = new QFrame(this);
    content->setFrameStyle(QFrame::Shape::StyledPanel);
    content->setLayout(new QVBoxLayout());
    content->setVisible(false);

    vBox->addWidget(content);

    std::vector<std::pair<void*, ComponentVariableInfo>> variables = GET_COMPONENT_VARS(componentTypeID, componentPtr);
    for(auto& member : variables)
    	AppendVariableWidget(member.second, member.first);

    /* Regularly update displayed values */

    QTimer* timer = new QTimer(this);
    timer->setInterval(100);
    QObject::connect(timer, &QTimer::timeout, this,
        [this]()
        {
            if(EntityManager::IsComponentValid(this->entity, this->componentTypeID))
    		    UpdateDisplayedVariables();
        });
    timer->start();
}
ComponentEntry::~ComponentEntry()
{

}

void ComponentEntry::AppendVariableWidget(const ComponentVariableInfo& variableInfo, void* variablePtr)
{
    ComponentVariableEntry* varEntry = nullptr;
    if(variableInfo.variableType == typeid(float)) varEntry = new ComponentVariableEntry_Float(variableInfo.variableName, variablePtr, this);
    if(variableInfo.variableType == typeid(glm::vec2)) varEntry = new ComponentVariableEntry_Vector2(variableInfo.variableName, variablePtr, this);
    if(variableInfo.variableType == typeid(glm::vec3)) varEntry = new ComponentVariableEntry_Vector3(variableInfo.variableName, variablePtr, this);
    if(variableInfo.variableType == typeid(glm::quat)) varEntry = new ComponentVariableEntry_QuaternionEuler(variableInfo.variableName, variablePtr, this);
    if(variableInfo.variableType == typeid(MeshRef)) varEntry = new ComponentVariableEntry_Mesh(variableInfo.variableName, variablePtr, this);
    if(variableInfo.variableType == typeid(MaterialRef)) varEntry = new ComponentVariableEntry_Material(variableInfo.variableName, variablePtr, this);
    	
    assert(varEntry && "Serialized component variable's type is not registered!");
    
    variableEntries.push_back(varEntry);
    content->layout()->addWidget(varEntry);
}

void ComponentEntry::UpdateDisplayedVariables()
{
    for(ComponentVariableEntry* varEntry : variableEntries)
	    varEntry->UpdateDisplayedValue();
}