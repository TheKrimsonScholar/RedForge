//#include "ComponentVariableEntry_Float.h"
//
//#include <glibmm.h>
//
//ComponentVariableEntry_Float::ComponentVariableEntry_Float(const std::string& label, void* variablePtr) : ComponentVariableEntry(label, variablePtr),
//	variablePtr(static_cast<float*>(variablePtr)), 
//	field(*this->variablePtr, 1.0, -FLT_MAX, FLT_MAX, 1)
//{
//	field.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Float::OnValueChanged));
//
//	append(field);
//}
//ComponentVariableEntry_Float::~ComponentVariableEntry_Float()
//{
//
//}
//
//void ComponentVariableEntry_Float::UpdateDisplayedValue()
//{
//	field.set_value(*variablePtr);
//}
//void ComponentVariableEntry_Float::OnValueChanged()
//{
//	*variablePtr = field.get_value();
//}