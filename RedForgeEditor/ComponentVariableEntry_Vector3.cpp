#include "ComponentVariableEntry_Vector3.h"

#include "DebugMacros.h"

#include <QLayout>

ComponentVariableEntry_Vector3::ComponentVariableEntry_Vector3(const std::string& label, void* variablePtr, QWidget* parent) : ComponentVariableEntry(label, variablePtr, parent),
	variablePtr(static_cast<glm::vec3*>(variablePtr))
{
	fieldX = new DragFloat(this->variablePtr->x, this);
	QObject::connect(fieldX, &DragFloat::valueChanged,
		[this](float value)
		{
			OnValueChanged();
		});
	fieldY = new DragFloat(this->variablePtr->y, this);
	QObject::connect(fieldY, &DragFloat::valueChanged,
		[this](float value)
		{
			OnValueChanged();
		});
	fieldZ = new DragFloat(this->variablePtr->z, this);
	QObject::connect(fieldZ, &DragFloat::valueChanged,
		[this](float value)
		{
			OnValueChanged();
		});

	layout()->addWidget(fieldX);
	layout()->addWidget(fieldY);
	layout()->addWidget(fieldZ);
}
ComponentVariableEntry_Vector3::~ComponentVariableEntry_Vector3()
{

}

void ComponentVariableEntry_Vector3::UpdateDisplayedValue()
{
	fieldX->SetValue(variablePtr->x);
	fieldY->SetValue(variablePtr->y);
	fieldZ->SetValue(variablePtr->z);
}
void ComponentVariableEntry_Vector3::OnValueChanged()
{
	variablePtr->x = fieldX->GetValue();
	variablePtr->y = fieldY->GetValue();
	variablePtr->z = fieldZ->GetValue();
}