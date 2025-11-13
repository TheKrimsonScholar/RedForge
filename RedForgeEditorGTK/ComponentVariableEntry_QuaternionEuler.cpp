//#include "ComponentVariableEntry_QuaternionEuler.h"
//
//#include <glibmm.h>
//
//ComponentVariableEntry_QuaternionEuler::ComponentVariableEntry_QuaternionEuler(const std::string& label, void* variablePtr) : ComponentVariableEntry(label, variablePtr),
//	variablePtr(static_cast<glm::quat*>(variablePtr)), 
//	fieldPitch(glm::pitch(*this->variablePtr), 1.0, -FLT_MAX, FLT_MAX, 1),
//	fieldYaw(glm::yaw(*this->variablePtr), 1.0, -FLT_MAX, FLT_MAX, 1),
//	fieldRoll(glm::roll(*this->variablePtr), 1.0, -FLT_MAX, FLT_MAX, 1)
//{
//	fieldPitch.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_QuaternionEuler::OnValueChanged));
//	fieldYaw.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_QuaternionEuler::OnValueChanged));
//	fieldRoll.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_QuaternionEuler::OnValueChanged));
//
//	append(fieldPitch);
//	append(fieldYaw);
//	append(fieldRoll);
//}
//ComponentVariableEntry_QuaternionEuler::~ComponentVariableEntry_QuaternionEuler()
//{
//	
//}
//
//void ComponentVariableEntry_QuaternionEuler::UpdateDisplayedValue()
//{
//	fieldPitch.set_value(glm::pitch(*variablePtr));
//	fieldYaw.set_value(glm::yaw(*variablePtr));
//	fieldRoll.set_value(glm::roll(*variablePtr));
//}
//void ComponentVariableEntry_QuaternionEuler::OnValueChanged()
//{
//	*variablePtr = glm::quat(glm::vec3(
//		fieldPitch.get_value(), 
//		fieldYaw.get_value(), 
//		fieldRoll.get_value()));
//}