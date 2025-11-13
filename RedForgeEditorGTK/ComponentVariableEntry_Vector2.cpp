//#include "ComponentVariableEntry_Vector2.h"
//
//#include <glibmm.h>
//
//ComponentVariableEntry_Vector2::ComponentVariableEntry_Vector2(const std::string& label, void* variablePtr) : ComponentVariableEntry(label, variablePtr),
//	variablePtr(static_cast<glm::vec2*>(variablePtr)), 
//	fieldX(this->variablePtr->x, 1.0, -FLT_MAX, FLT_MAX, 1), fieldY(this->variablePtr->y, 1.0, -FLT_MAX, FLT_MAX, 1)
//{
//	fieldX.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Vector2::OnValueChanged));
//	fieldY.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Vector2::OnValueChanged));
//
//	append(fieldX);
//	append(fieldY);
//}
//ComponentVariableEntry_Vector2::~ComponentVariableEntry_Vector2()
//{
//
//}
//
//void ComponentVariableEntry_Vector2::UpdateDisplayedValue()
//{
//	fieldX.set_value(variablePtr->x);
//	fieldY.set_value(variablePtr->y);
//}
//void ComponentVariableEntry_Vector2::OnValueChanged()
//{
//	variablePtr->x = fieldX.get_value();
//	variablePtr->y = fieldY.get_value();
//}