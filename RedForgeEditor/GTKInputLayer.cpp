#include "GTKInputLayer.h"

#include <cassert>
#include <iostream>

#include <gtkmm.h>

static std::unordered_map<guint, MouseButtonCode> GDK_INPUT_MOUSE_BUTTON_MAP =
{
    { GDK_BUTTON_PRIMARY, MouseButtonCode::Left },
    { GDK_BUTTON_MIDDLE, MouseButtonCode::Middle },
    { GDK_BUTTON_SECONDARY, MouseButtonCode::Right }
};
static std::unordered_map<guint, KeyCode> GDK_INPUT_KEY_MAP =
{
    { GDK_KEY_a, KeyCode::A }, { GDK_KEY_A, KeyCode::A },
    { GDK_KEY_b, KeyCode::B }, { GDK_KEY_B, KeyCode::B },
    { GDK_KEY_c, KeyCode::C }, { GDK_KEY_C, KeyCode::C },
    { GDK_KEY_d, KeyCode::D }, { GDK_KEY_D, KeyCode::D },
    { GDK_KEY_e, KeyCode::E }, { GDK_KEY_E, KeyCode::E },
    { GDK_KEY_f, KeyCode::F }, { GDK_KEY_F, KeyCode::F },
    { GDK_KEY_g, KeyCode::G }, { GDK_KEY_G, KeyCode::G },
    { GDK_KEY_h, KeyCode::H }, { GDK_KEY_H, KeyCode::H },
    { GDK_KEY_i, KeyCode::I }, { GDK_KEY_I, KeyCode::I },
    { GDK_KEY_j, KeyCode::J }, { GDK_KEY_J, KeyCode::J },
    { GDK_KEY_k, KeyCode::K }, { GDK_KEY_K, KeyCode::K },
    { GDK_KEY_l, KeyCode::L }, { GDK_KEY_L, KeyCode::L },
    { GDK_KEY_m, KeyCode::M }, { GDK_KEY_M, KeyCode::M },
    { GDK_KEY_n, KeyCode::N }, { GDK_KEY_N, KeyCode::N },
    { GDK_KEY_o, KeyCode::O }, { GDK_KEY_O, KeyCode::O },
    { GDK_KEY_p, KeyCode::P }, { GDK_KEY_P, KeyCode::P },
    { GDK_KEY_q, KeyCode::Q }, { GDK_KEY_Q, KeyCode::Q },
    { GDK_KEY_r, KeyCode::R }, { GDK_KEY_R, KeyCode::R },
    { GDK_KEY_s, KeyCode::S }, { GDK_KEY_S, KeyCode::S },
    { GDK_KEY_t, KeyCode::T }, { GDK_KEY_T, KeyCode::T },
    { GDK_KEY_u, KeyCode::U }, { GDK_KEY_U, KeyCode::U },
    { GDK_KEY_v, KeyCode::V }, { GDK_KEY_V, KeyCode::V },
    { GDK_KEY_w, KeyCode::W }, { GDK_KEY_W, KeyCode::W },
    { GDK_KEY_x, KeyCode::X }, { GDK_KEY_X, KeyCode::X },
    { GDK_KEY_y, KeyCode::Y }, { GDK_KEY_Y, KeyCode::Y },
    { GDK_KEY_z, KeyCode::Z }, { GDK_KEY_Z, KeyCode::Z },

    { GDK_KEY_Shift_L, KeyCode::LSHIFT },
    { GDK_KEY_Shift_R, KeyCode::RSHIFT },
    { GDK_KEY_space, KeyCode::SPACE },
    { GDK_KEY_Escape, KeyCode::ESCAPE },
    { GDK_KEY_Control_L, KeyCode::LCTRL },
    { GDK_KEY_Control_R, KeyCode::RCTRL },
    { GDK_KEY_Alt_L, KeyCode::LALT },
    { GDK_KEY_Alt_R, KeyCode::RALT }
};

GTKInputLayer::GTKInputLayer(Gtk::Widget* widget) : 
	widget(widget)
{
    m_mouseController = Gtk::EventControllerMotion::create();
    m_mouseController->signal_motion().connect(sigc::mem_fun(*this, &GTKInputLayer::OnMouseMotion), false);

    m_clickGesture = Gtk::GestureClick::create();
    m_clickGesture->set_button(0); // Respond to all mouse buttons
    m_clickGesture->signal_pressed().connect(sigc::mem_fun(*this, &GTKInputLayer::OnMouseButtonPressed), false);
    m_clickGesture->signal_released().connect(sigc::mem_fun(*this, &GTKInputLayer::OnMouseButtonReleased), false);

    m_keyController = Gtk::EventControllerKey::create();
    m_keyController->signal_key_pressed().connect(sigc::mem_fun(*this, &GTKInputLayer::OnKeyPressed), false);
    m_keyController->signal_key_released().connect(sigc::mem_fun(*this, &GTKInputLayer::OnKeyReleased), false);

    widget->add_controller(m_clickGesture);
    widget->add_controller(m_keyController);
    widget->add_controller(m_mouseController);
}
GTKInputLayer::~GTKInputLayer()
{
    
}

void GTKInputLayer::PreUpdate()
{
    InputLayer::PreUpdate();
}
void GTKInputLayer::PostUpdate()
{
    InputLayer::PostUpdate();
}

void GTKInputLayer::OnMouseMotion(double x, double y)
{
    mousePosition = { x, y };
}

void GTKInputLayer::OnMouseButtonPressed(int n_press, double x, double y)
{
    auto event = m_clickGesture->get_current_event();

    MouseButtonCode mouseButton = GDK_INPUT_MOUSE_BUTTON_MAP[event->get_button()];

    mouseButtonsDown[(size_t) mouseButton] = true;
}
void GTKInputLayer::OnMouseButtonReleased(int n_press, double x, double y)
{
    auto event = m_clickGesture->get_current_event();

    MouseButtonCode mouseButton = GDK_INPUT_MOUSE_BUTTON_MAP[event->get_button()];

    mouseButtonsDown[(size_t) mouseButton] = false;
}

bool GTKInputLayer::OnKeyPressed(guint keyval, guint keycode, Gdk::ModifierType state)
{
    assert(GDK_INPUT_KEY_MAP.find(keyval) != GDK_INPUT_KEY_MAP.end() && "GDK key isn't registered.");

    KeyCode key = GDK_INPUT_KEY_MAP[keyval];

    keysDown[(size_t) key] = true;
    keysPressedThisFrame[(size_t) key] = true;

    return true;
}
void GTKInputLayer::OnKeyReleased(guint keyval, guint keycode, Gdk::ModifierType state)
{
    assert(GDK_INPUT_KEY_MAP.find(keyval) != GDK_INPUT_KEY_MAP.end() && "GDK key isn't registered.");

    KeyCode key = GDK_INPUT_KEY_MAP[keyval];

    keysDown[(size_t) key] = false;
    keysReleasedThisFrame[(size_t) key] = true;
}