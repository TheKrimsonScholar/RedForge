#pragma once

#include "ComponentVariableEntry.h"

#include "MeshRef.h"

#include "SearchField.h"

class ComponentVariableEntry_Mesh : public ComponentVariableEntry
{
	Q_OBJECT

private:
	MeshRef* variablePtr;

	SearchField* searchField;

	std::vector<MeshRef> meshes;

public:
	ComponentVariableEntry_Mesh(const std::string& label, void* variablePtr, QWidget* parent = nullptr);
	~ComponentVariableEntry_Mesh();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};