//#include "ComponentVariableEntry_Vector3.h"
//
//#include "DebugMacros.h"
//
//#include <glibmm.h>
//
//ComponentVariableEntry_Vector3::ComponentVariableEntry_Vector3(const std::string& label, void* variablePtr) : ComponentVariableEntry(label, variablePtr),
//	variablePtr(static_cast<glm::vec3*>(variablePtr)), 
//	fieldX(this->variablePtr->x, 1.0, -FLT_MAX, FLT_MAX, 3), fieldY(this->variablePtr->y, 1.0, -FLT_MAX, FLT_MAX, 3), fieldZ(this->variablePtr->z, 1.0, -FLT_MAX, FLT_MAX, 3)
//{
//	////spinButtonX = new Gtk::SpinButton(adjustment, 0.4f, 3);
//	//spinButtonX.set_adjustment(adjustment);
//	//spinButtonX.set_climb_rate(0.4f);
//	//spinButtonX.set_digits(2);
//	//spinButtonX.signal_value_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Vector3::OnValueChanged), false);
//	///*timeoutConnection = Glib::signal_timeout().connect([this, &x]() -> bool
//	//	{
//	//		spinButtonX->set_value(x);
//	//		return true;
//	//	}, 2000);*/
//
//	//spinButtonY.set_adjustment(adjustment);
//	//spinButtonY.set_climb_rate(0.4f);
//	//spinButtonY.set_digits(2);
//	//spinButtonY.signal_value_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Vector3::OnValueChanged), false);
//	///*timeoutConnection = Glib::signal_timeout().connect([this, &x]() -> bool
//	//	{
//	//		spinButtonX->set_value(x);
//	//		return true;
//	//	}, 2000);*/
//
//	//spinButtonZ.set_adjustment(adjustment);
//	//spinButtonZ.set_climb_rate(0.4f);
//	//spinButtonZ.set_digits(2);
//	//spinButtonZ.signal_value_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Vector3::OnValueChanged), false);
//	///*timeoutConnection = Glib::signal_timeout().connect([this, &x]() -> bool
//	//	{
//	//		spinButtonX->set_value(x);
//	//		return true;
//	//	}, 2000);*/
//
//	fieldX.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Vector3::OnValueChanged));
//	fieldY.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Vector3::OnValueChanged));
//	fieldZ.signal_changed().connect(sigc::mem_fun(*this, &ComponentVariableEntry_Vector3::OnValueChanged));
//
//	append(fieldX);
//	append(fieldY);
//	append(fieldZ);
//}
//ComponentVariableEntry_Vector3::~ComponentVariableEntry_Vector3()
//{
//
//}
//
//void ComponentVariableEntry_Vector3::UpdateDisplayedValue()
//{
//	fieldX.set_value(variablePtr->x);
//	fieldY.set_value(variablePtr->y);
//	fieldZ.set_value(variablePtr->z);
//}
//void ComponentVariableEntry_Vector3::OnValueChanged()
//{
//	variablePtr->x = fieldX.get_value();
//	variablePtr->y = fieldY.get_value();
//	variablePtr->z = fieldZ.get_value();
//}