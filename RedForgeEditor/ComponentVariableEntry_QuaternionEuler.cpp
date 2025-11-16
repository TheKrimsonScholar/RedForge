#include "ComponentVariableEntry_QuaternionEuler.h"

#include <QLayout>

ComponentVariableEntry_QuaternionEuler::ComponentVariableEntry_QuaternionEuler(const std::string& label, void* variablePtr, QWidget* parent) : ComponentVariableEntry(label, variablePtr, parent),
	variablePtr(static_cast<glm::quat*>(variablePtr))
{
	fieldPitch = new DragFloat(glm::pitch(*this->variablePtr), this);
	QObject::connect(fieldPitch, &DragFloat::valueChanged,
		[this](float value)
		{
			OnValueChanged();
		});
	fieldYaw = new DragFloat(glm::yaw(*this->variablePtr), this);
	QObject::connect(fieldYaw, &DragFloat::valueChanged,
		[this](float value)
		{
			OnValueChanged();
		});
	fieldRoll = new DragFloat(glm::roll(*this->variablePtr), this);
	QObject::connect(fieldRoll, &DragFloat::valueChanged,
		[this](float value)
		{
			OnValueChanged();
		});

	layout()->addWidget(fieldPitch);
	layout()->addWidget(fieldYaw);
	layout()->addWidget(fieldRoll);
}
ComponentVariableEntry_QuaternionEuler::~ComponentVariableEntry_QuaternionEuler()
{
	
}

void ComponentVariableEntry_QuaternionEuler::UpdateDisplayedValue()
{
	if(fieldPitch->GetValue() != glm::pitch(*variablePtr)) fieldPitch->SetValue(glm::pitch(*variablePtr));
	if(fieldYaw->GetValue() != glm::yaw(*variablePtr)) fieldYaw->SetValue(glm::yaw(*variablePtr));
	if(fieldRoll->GetValue() != glm::roll(*variablePtr)) fieldRoll->SetValue(glm::roll(*variablePtr));
}
void ComponentVariableEntry_QuaternionEuler::OnValueChanged()
{
	*variablePtr = glm::quat(glm::vec3(
		fieldPitch->GetValue(), 
		fieldYaw->GetValue(), 
		fieldRoll->GetValue()));
}