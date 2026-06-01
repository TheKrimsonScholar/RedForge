#include "ComponentVariableEntry_Material.h"

#include "World.h"

#include "Assets.h"

#include "PathUtils.h"

#include <QLayout>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

ComponentVariableEntry_Material::ComponentVariableEntry_Material(const std::string& label, void* variablePtr, QWidget* parent) : ComponentVariableEntry(label, variablePtr, parent),
	variablePtr(static_cast<MaterialRef*>(variablePtr))
{
	int initialMeshIndex = -1;
	QStringList materialNames;
	for(const Material* material : Editor::GetWorld().GetResource<Assets>().GetMaterials())
	{
		// If this is the initially selected material, remember the index
		if(material->identifier == this->variablePtr->identifier)
			initialMeshIndex = materials.size();

		materialNames << QString(WideToNarrow(material->identifier).c_str());
		materials.emplace_back(material->identifier);
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