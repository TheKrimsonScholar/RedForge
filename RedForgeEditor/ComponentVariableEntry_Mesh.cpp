#include "ComponentVariableEntry_Mesh.h"

#include "ResourceManager.h"
#include "PathUtils.h"

#include <QLayout>

ComponentVariableEntry_Mesh::ComponentVariableEntry_Mesh(const std::string& label, void* variablePtr, QWidget* parent) : ComponentVariableEntry(label, variablePtr, parent),
	variablePtr(static_cast<MeshRef*>(variablePtr))
{
	int initialMeshIndex = -1;
	QStringList meshNames;
	for(auto& mesh : ResourceManager::GetMeshMap())
	{
		// If this is the initially selected mesh, remember the index
		if(mesh.first == this->variablePtr->identifier)
			initialMeshIndex = meshes.size();

		meshNames << QString(WideToNarrow(mesh.first).c_str());
		meshes.emplace_back(mesh.first);
	}

	searchField = new SearchField("Mesh", this);
	searchField->SetItems(meshNames);
	searchField->SetSelectedIndex(initialMeshIndex);
	QObject::connect(searchField, &SearchField::OnSelectionChanged,
		[this](int index)
		{
			OnValueChanged();
		});

	layout()->addWidget(searchField);
}
ComponentVariableEntry_Mesh::~ComponentVariableEntry_Mesh()
{

}

void ComponentVariableEntry_Mesh::UpdateDisplayedValue()
{

}
void ComponentVariableEntry_Mesh::OnValueChanged()
{
	*variablePtr = meshes[searchField->GetSelectedIndex()];
}