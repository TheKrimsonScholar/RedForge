#include "ComponentVariableEntry_Float.h"

#include <QLayout>
#include <QLineEdit>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

ComponentVariableEntry_Float::ComponentVariableEntry_Float(const std::string& label, void* variablePtr, QWidget* parent) : ComponentVariableEntry(label, variablePtr, parent),
	variablePtr(static_cast<float*>(variablePtr))
{
	field = new DragFloat(*this->variablePtr, this);
	QObject::connect(field, &DragFloat::valueChanged,
		[this](float value)
		{
			OnValueChanged();
		});

	layout()->addWidget(field);
}
ComponentVariableEntry_Float::~ComponentVariableEntry_Float()
{

}

void ComponentVariableEntry_Float::UpdateDisplayedValue()
{
	if(field->GetValue() != *variablePtr) field->SetValue(*variablePtr);
}
void ComponentVariableEntry_Float::OnValueChanged()
{
	*variablePtr = field->GetValue();
}