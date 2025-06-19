#pragma once

#include "ComponentVariableEntry.h"

#include <memory>

#include <glm/glm.hpp>

#include "NumberField.h"

class ComponentVariableEntry_Vector3 : public ComponentVariableEntry
{
private:
	glm::vec3* variablePtr;

	DragFloat fieldX;
	DragFloat fieldY;
	DragFloat fieldZ;

public:
	ComponentVariableEntry_Vector3(const std::string& label, void* variablePtr);
	~ComponentVariableEntry_Vector3();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};