#pragma once

#include "ComponentVariableEntry.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "DragFloat.h"

class ComponentVariableEntry_QuaternionEuler : public ComponentVariableEntry
{
	Q_OBJECT

private:
	glm::quat* variablePtr;

	DragFloat* fieldPitch;
	DragFloat* fieldYaw;
	DragFloat* fieldRoll;

public:
	ComponentVariableEntry_QuaternionEuler(const std::string& label, void* variablePtr, QWidget* parent = nullptr);
	~ComponentVariableEntry_QuaternionEuler();

private:
	virtual void UpdateDisplayedValue() override;
	virtual void OnValueChanged() override;
};