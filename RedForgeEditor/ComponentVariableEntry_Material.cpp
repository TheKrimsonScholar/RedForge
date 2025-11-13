#include "ComponentVariableEntry_Material.h"

#include "ResourceManager.h"
#include "PathUtils.h"

#include <QLayout>

ComponentVariableEntry_Material::ComponentVariableEntry_Material(const std::string& label, void* variablePtr, QWidget* parent) : ComponentVariableEntry(label, variablePtr, parent),
	variablePtr(static_cast<MaterialRef*>(variablePtr))
{
	int initialMeshIndex = -1;
	QStringList materialNames;
	for(auto& material : ResourceManager::GetMaterialMap())
	{
		// If this is the initially selected material, remember the index
		if(material.first == this->variablePtr->identifier)
			initialMeshIndex = materials.size();

		materialNames << QString(WideToNarrow(material.first).c_str());
		materials.emplace_back(material.first);
	}

	searchField = new SearchField("Mesh", this);
	searchField->SetItems(materialNames);
	searchField->SetSelectedIndex(initialMeshIndex);
	QObject::connect(searchField, &SearchField::OnSelectionChanged,
		[this](int index)
		{
			OnValueChanged();
		});

	layout()->addWidget(searchField);
}
ComponentVariableEntry_Material::~ComponentVariableEntry_Material()
{

}

void ComponentVariableEntry_Material::UpdateDisplayedValue()
{

}
void ComponentVariableEntry_Material::OnValueChanged()
{
	*variablePtr = materials[searchField->GetSelectedIndex()];
}