#pragma once

#include "ComponentVariableEntry.h"

#include <glm/glm.hpp>

#include "DragFloat.h"

class ComponentVariableEntry_Vector2 : public ComponentVariableEntry
{
	Q_OBJECT

private:
	glm::vec2* variablePtr;

	DragFloat* fieldX;
	DragFloat* fieldY;

public:
	ComponentVariableEntry_Vector2(const std::string& label, void* variablePtr, QWidget* parent = nullptr);
	~ComponentVariableEntry_Vector2();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};