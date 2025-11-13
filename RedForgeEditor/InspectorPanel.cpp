#include "InspectorPanel.h"

#include <QFrame>
#include <QToolButton>
#include <QGroupBox>

#include "QtUtils.h"
#include "ComponentEntry.h"
#include "SearchField.h"

InspectorPanel::InspectorPanel(QWidget* parent) : EditorPanel("Inspector", parent)
{
	label = new QLabel("None", this);

	componentEntriesBox = new QVBoxLayout();

	QVBoxLayout* vBox = new QVBoxLayout();
	vBox->setAlignment(Qt::AlignTop);
	vBox->addWidget(label);
	vBox->addLayout(componentEntriesBox);

	contentArea->setLayout(vBox);
}
InspectorPanel::~InspectorPanel()
{

}

void InspectorPanel::Initialize()
{

}
void InspectorPanel::Update()
{

}

void InspectorPanel::SetTarget(const Entity& entity)
{
	label->setText(LevelManager::GetEntityName(entity).c_str());
	
	// Clear any existing component entries
	clearLayout(componentEntriesBox);
	
	std::unordered_map<void*, std::type_index> components = EntityManager::GetAllComponents(entity);
	std::vector<std::type_index> registeredComponentTypes = GetRegisteredComponentsList();
	// List this entity's components in order by type
	for(std::type_index componentType : registeredComponentTypes)
		for(auto& component : components)
		{
			if(component.second != componentType)
				continue;

			ComponentEntry* componentEntry = new ComponentEntry(entity, component.second, component.first, this);
			componentEntriesBox->addWidget(componentEntry);
				
			// Create remove component button
			//Gtk::Button* removeComponentButton = Gtk::manage(new Gtk::Button("X"));
			//removeComponentButton->set_halign(Gtk::Align::END);
			//removeComponentButton->signal_clicked().connect([this, entity, componentType]()
			//	{
			//		EntityManager::RemoveComponentOfType(entity, componentType);
	
			//		// Refresh the inspector
			//		SetTarget(entity);
			//	});
			//	
			//componentEntry->append(*newEntry);
			//componentEntry->append(*spacer);
			//componentEntry->append(*removeComponentButton);
			//	
			//componentsList.append(*componentEntry);
		}

	/* Search field for adding new components to the entity */
	
	QStringList componentNames;
	// Get the names of all registered component types
	for(std::type_index componentTypeIndex : GetRegisteredComponentsList())
		componentNames << QString(GET_COMPONENT_NAME(componentTypeIndex).c_str());

	SearchField* addComponentField = new SearchField("Add Component...", this);
	addComponentField->SetItems(componentNames);
	QObject::connect(addComponentField, &SearchField::OnSelectionChanged,
		[this, entity, addComponentField](int index)
		{
			EntityManager::AddComponentOfType(entity, GetRegisteredComponentsList()[index]);
			
			// Refresh the inspector
			SetTarget(entity);
		});

	componentEntriesBox->addWidget(addComponentField);
}

void InspectorPanel::ResetTarget()
{
	label->setText("None");

	// Clear any existing component entries
	clearLayout(componentEntriesBox);
}