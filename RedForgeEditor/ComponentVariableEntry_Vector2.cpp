#include "ComponentVariableEntry_Vector2.h"

#include <QLayout>

ComponentVariableEntry_Vector2::ComponentVariableEntry_Vector2(const std::string& label, void* variablePtr, QWidget* parent) : ComponentVariableEntry(label, variablePtr, parent),
	variablePtr(static_cast<glm::vec2*>(variablePtr))
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

	layout()->addWidget(fieldX);
	layout()->addWidget(fieldY);
}
ComponentVariableEntry_Vector2::~ComponentVariableEntry_Vector2()
{

}

void ComponentVariableEntry_Vector2::UpdateDisplayedValue()
{
	fieldX->SetValue(variablePtr->x);
	fieldY->SetValue(variablePtr->y);
}
void ComponentVariableEntry_Vector2::OnValueChanged()
{
	variablePtr->x = fieldX->GetValue();
	variablePtr->y = fieldY->GetValue();
}