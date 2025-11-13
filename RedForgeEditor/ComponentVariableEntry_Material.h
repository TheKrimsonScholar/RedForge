#pragma once

#include "ComponentVariableEntry.h"

#include "MaterialRef.h"

#include "SearchField.h"

class ComponentVariableEntry_Material : public ComponentVariableEntry
{
	Q_OBJECT

private:
	MaterialRef* variablePtr;

	SearchField* searchField;

	std::vector<MaterialRef> materials;

public:
	ComponentVariableEntry_Material(const std::string& label, void* variablePtr, QWidget* parent = nullptr);
	~ComponentVariableEntry_Material();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};