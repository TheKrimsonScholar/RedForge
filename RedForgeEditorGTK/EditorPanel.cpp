//#include "EditorPanel.h"
//
//EditorPanel::EditorPanel(const std::string& panelLabel) : Gtk::Box(Gtk::Orientation::VERTICAL), 
//	windowLabel(panelLabel), scrollArea(), contentArea(Gtk::Orientation::VERTICAL)
//{
//	windowLabel.add_css_class("panel-label");
//	windowLabel.set_justify(Gtk::Justification::LEFT);
//	windowLabel.set_wrap(true);
//
//	scrollArea.add_css_class("panel-scrollarea");
//	scrollArea.set_vexpand(true);
//	scrollArea.set_child(contentArea);
//
//	append(windowLabel);
//	append(scrollArea);
//
//	clickGesture = Gtk::GestureClick::create();
//	clickGesture->set_button(0); // Respond to all mouse buttons
//	// Setup manual focusing when clicking in panel region
//	clickGesture->signal_pressed().connect([this](int presses, double x, double y) { scrollArea.grab_focus(); }, false);
//
//	add_controller(clickGesture);
//}
//EditorPanel::~EditorPanel()
//{
//
//}