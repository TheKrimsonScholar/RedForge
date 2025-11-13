#pragma once

#include "ComponentVariableEntry.h"

#include <glm/glm.hpp>

#include "DragFloat.h"

class ComponentVariableEntry_Vector3 : public ComponentVariableEntry
{
	Q_OBJECT

private:
	glm::vec3* variablePtr;

	DragFloat* fieldX;
	DragFloat* fieldY;
	DragFloat* fieldZ;

public:
	ComponentVariableEntry_Vector3(const std::string& label, void* variablePtr, QWidget* parent = nullptr);
	~ComponentVariableEntry_Vector3();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};