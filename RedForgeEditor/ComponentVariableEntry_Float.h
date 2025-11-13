#pragma once

#include "ComponentVariableEntry.h"

#include "DragFloat.h"

class ComponentVariableEntry_Float : public ComponentVariableEntry
{
	Q_OBJECT

private:
	float* variablePtr;

	DragFloat* field;

public:
	ComponentVariableEntry_Float(const std::string& label, void* variablePtr, QWidget* parent = nullptr);
	~ComponentVariableEntry_Float();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};