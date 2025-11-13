//#include "GTKInputLayer.h"
//
//#include <cassert>
//#include <iostream>
//
//#include <gtkmm.h>
//
//static std::unordered_map<guint, MouseButtonCode> GDK_INPUT_MOUSE_BUTTON_MAP =
//{
//    { GDK_BUTTON_PRIMARY, MouseButtonCode::Left },
//    { GDK_BUTTON_MIDDLE, MouseButtonCode::Middle },
//    { GDK_BUTTON_SECONDARY, MouseButtonCode::Right }
//};
//static std::unordered_map<guint, RFKeyCode> GDK_INPUT_KEY_MAP =
//{
//    { GDK_KEY_a, RFKeyCode::A }, { GDK_KEY_A, RFKeyCode::A },
//    { GDK_KEY_b, RFKeyCode::B }, { GDK_KEY_B, RFKeyCode::B },
//    { GDK_KEY_c, RFKeyCode::C }, { GDK_KEY_C, RFKeyCode::C },
//    { GDK_KEY_d, RFKeyCode::D }, { GDK_KEY_D, RFKeyCode::D },
//    { GDK_KEY_e, RFKeyCode::E }, { GDK_KEY_E, RFKeyCode::E },
//    { GDK_KEY_f, RFKeyCode::F }, { GDK_KEY_F, RFKeyCode::F },
//    { GDK_KEY_g, RFKeyCode::G }, { GDK_KEY_G, RFKeyCode::G },
//    { GDK_KEY_h, RFKeyCode::H }, { GDK_KEY_H, RFKeyCode::H },
//    { GDK_KEY_i, RFKeyCode::I }, { GDK_KEY_I, RFKeyCode::I },
//    { GDK_KEY_j, RFKeyCode::J }, { GDK_KEY_J, RFKeyCode::J },
//    { GDK_KEY_k, RFKeyCode::K }, { GDK_KEY_K, RFKeyCode::K },
//    { GDK_KEY_l, RFKeyCode::L }, { GDK_KEY_L, RFKeyCode::L },
//    { GDK_KEY_m, RFKeyCode::M }, { GDK_KEY_M, RFKeyCode::M },
//    { GDK_KEY_n, RFKeyCode::N }, { GDK_KEY_N, RFKeyCode::N },
//    { GDK_KEY_o, RFKeyCode::O }, { GDK_KEY_O, RFKeyCode::O },
//    { GDK_KEY_p, RFKeyCode::P }, { GDK_KEY_P, RFKeyCode::P },
//    { GDK_KEY_q, RFKeyCode::Q }, { GDK_KEY_Q, RFKeyCode::Q },
//    { GDK_KEY_r, RFKeyCode::R }, { GDK_KEY_R, RFKeyCode::R },
//    { GDK_KEY_s, RFKeyCode::S }, { GDK_KEY_S, RFKeyCode::S },
//    { GDK_KEY_t, RFKeyCode::T }, { GDK_KEY_T, RFKeyCode::T },
//    { GDK_KEY_u, RFKeyCode::U }, { GDK_KEY_U, RFKeyCode::U },
//    { GDK_KEY_v, RFKeyCode::V }, { GDK_KEY_V, RFKeyCode::V },
//    { GDK_KEY_w, RFKeyCode::W }, { GDK_KEY_W, RFKeyCode::W },
//    { GDK_KEY_x, RFKeyCode::X }, { GDK_KEY_X, RFKeyCode::X },
//    { GDK_KEY_y, RFKeyCode::Y }, { GDK_KEY_Y, RFKeyCode::Y },
//    { GDK_KEY_z, RFKeyCode::Z }, { GDK_KEY_Z, RFKeyCode::Z },
//
//    { GDK_KEY_Shift_L, RFKeyCode::LSHIFT },
//    { GDK_KEY_Shift_R, RFKeyCode::RSHIFT },
//    { GDK_KEY_space, RFKeyCode::SPACE },
//    { GDK_KEY_Escape, RFKeyCode::ESCAPE },
//    { GDK_KEY_Control_L, RFKeyCode::LCTRL },
//    { GDK_KEY_Control_R, RFKeyCode::RCTRL },
//    { GDK_KEY_Alt_L, RFKeyCode::LALT },
//    { GDK_KEY_Alt_R, RFKeyCode::RALT }
//};
//
//GTKInputLayer::GTKInputLayer(Gtk::Widget* widget) : 
//	widget(widget)
//{
//    m_mouseController = Gtk::EventControllerMotion::create();
//    m_mouseController->signal_motion().connect(sigc::mem_fun(*this, &GTKInputLayer::OnMouseMotion), false);
//
//    m_clickGesture = Gtk::GestureClick::create();
//    m_clickGesture->set_button(0); // Respond to all mouse buttons
//    m_clickGesture->signal_pressed().connect(sigc::mem_fun(*this, &GTKInputLayer::OnMouseButtonPressed), false);
//    m_clickGesture->signal_released().connect(sigc::mem_fun(*this, &GTKInputLayer::OnMouseButtonReleased), false);
//
//    m_keyController = Gtk::EventControllerKey::create();
//    m_keyController->signal_key_pressed().connect(sigc::mem_fun(*this, &GTKInputLayer::OnKeyPressed), false);
//    m_keyController->signal_key_released().connect(sigc::mem_fun(*this, &GTKInputLayer::OnKeyReleased), false);
//
//    widget->add_controller(m_clickGesture);
//    widget->add_controller(m_keyController);
//    widget->add_controller(m_mouseController);
//}
//GTKInputLayer::~GTKInputLayer()
//{
//    
//}
//
//void GTKInputLayer::PreUpdate()
//{
//    InputLayer::PreUpdate();
//}
//void GTKInputLayer::PostUpdate()
//{
//    InputLayer::PostUpdate();
//}
//
//void GTKInputLayer::OnMouseMotion(double x, double y)
//{
//    mousePosition = { x, y };
//}
//
//void GTKInputLayer::OnMouseButtonPressed(int n_press, double x, double y)
//{
//    // Manually grab focus for the viewport whenever a mouse button is pressed
//    widget->grab_focus();
//
//    auto event = m_clickGesture->get_current_event();
//
//    MouseButtonCode mouseButton = GDK_INPUT_MOUSE_BUTTON_MAP[event->get_button()];
//
//    mouseButtonsDown[(size_t) mouseButton] = true;
//}
//void GTKInputLayer::OnMouseButtonReleased(int n_press, double x, double y)
//{
//    auto event = m_clickGesture->get_current_event();
//
//    MouseButtonCode mouseButton = GDK_INPUT_MOUSE_BUTTON_MAP[event->get_button()];
//
//    mouseButtonsDown[(size_t) mouseButton] = false;
//}
//
//bool GTKInputLayer::OnKeyPressed(guint keyval, guint keycode, Gdk::ModifierType state)
//{
//    assert(GDK_INPUT_KEY_MAP.find(keyval) != GDK_INPUT_KEY_MAP.end() && "GDK key isn't registered.");
//
//    RFKeyCode key = GDK_INPUT_KEY_MAP[keyval];
//
//    keysDown[(size_t) key] = true;
//    keysPressedThisFrame[(size_t) key] = true;
//
//    return true;
//}
//void GTKInputLayer::OnKeyReleased(guint keyval, guint keycode, Gdk::ModifierType state)
//{
//    assert(GDK_INPUT_KEY_MAP.find(keyval) != GDK_INPUT_KEY_MAP.end() && "GDK key isn't registered.");
//
//    RFKeyCode key = GDK_INPUT_KEY_MAP[keyval];
//
//    keysDown[(size_t) key] = false;
//    keysReleasedThisFrame[(size_t) key] = true;
//}