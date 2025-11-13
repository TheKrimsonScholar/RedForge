#include "QtInputLayer.h"

#include <cassert>
#include <iostream>

#include <QMouseEvent>
#include <QKeyEvent>

static std::unordered_map<Qt::MouseButton, MouseButtonCode> QT_INPUT_MOUSE_BUTTON_MAP =
{
    { Qt::LeftButton, MouseButtonCode::Left },
    { Qt::MiddleButton, MouseButtonCode::Middle },
    { Qt::RightButton, MouseButtonCode::Right }
};
static std::unordered_map<int, RFKeyCode> QT_INPUT_KEY_MAP =
{
    { Qt::Key_A, RFKeyCode::A },
    { Qt::Key_B, RFKeyCode::B },
    { Qt::Key_C, RFKeyCode::C },
    { Qt::Key_D, RFKeyCode::D },
    { Qt::Key_E, RFKeyCode::E },
    { Qt::Key_F, RFKeyCode::F },
    { Qt::Key_G, RFKeyCode::G },
    { Qt::Key_H, RFKeyCode::H },
    { Qt::Key_I, RFKeyCode::I },
    { Qt::Key_J, RFKeyCode::J },
    { Qt::Key_K, RFKeyCode::K },
    { Qt::Key_L, RFKeyCode::L },
    { Qt::Key_M, RFKeyCode::M },
    { Qt::Key_N, RFKeyCode::N },
    { Qt::Key_O, RFKeyCode::O },
    { Qt::Key_P, RFKeyCode::P },
    { Qt::Key_Q, RFKeyCode::Q },
    { Qt::Key_R, RFKeyCode::R },
    { Qt::Key_S, RFKeyCode::S },
    { Qt::Key_T, RFKeyCode::T },
    { Qt::Key_U, RFKeyCode::U },
    { Qt::Key_V, RFKeyCode::V },
    { Qt::Key_W, RFKeyCode::W },
    { Qt::Key_X, RFKeyCode::X },
    { Qt::Key_Y, RFKeyCode::Y },
    { Qt::Key_Z, RFKeyCode::Z },

    { Qt::Key_Shift, RFKeyCode::LSHIFT },
    { Qt::Key_Shift, RFKeyCode::RSHIFT },
    { Qt::Key_Space, RFKeyCode::SPACE },
    { Qt::Key_Escape, RFKeyCode::ESCAPE },
    { Qt::Key_Control, RFKeyCode::LCTRL },
    { Qt::Key_Control, RFKeyCode::RCTRL },
    { Qt::Key_Alt, RFKeyCode::LALT },
    { Qt::Key_Alt, RFKeyCode::RALT }
};

QtInputLayer::QtInputLayer(QWindow* window)
    : window(window)
{
    window->installEventFilter(this);
}
QtInputLayer::~QtInputLayer()
{
    
}

void QtInputLayer::PreUpdate()
{
    InputLayer::PreUpdate();
}
void QtInputLayer::PostUpdate()
{
    InputLayer::PostUpdate();
}

bool QtInputLayer::eventFilter(QObject* watched, QEvent* event)
{
    switch(event->type())
    {
        case QEvent::MouseMove:
            OnMouseMove(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseButtonPress:
            OnMouseButtonPressed(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::MouseButtonRelease:
            OnMouseButtonReleased(static_cast<QMouseEvent*>(event));
            break;
        case QEvent::KeyPress:
            OnKeyPressed(static_cast<QKeyEvent*>(event));
            break;
        case QEvent::KeyRelease:
            OnKeyReleased(static_cast<QKeyEvent*>(event));
            break;
        default: break;
    }

    return false;
}

void QtInputLayer::OnMouseMove(QMouseEvent* event)
{
    //std::cout << "MOUSE MOVE " << event->position().x() << ", " << event->position().y() << std::endl;

    // Convert from device-independent pixel coordinates to physical pixel coordinates
    mousePosition = { event->position().x() * window->devicePixelRatio(), event->position().y() * window->devicePixelRatio() };
}

void QtInputLayer::OnMouseButtonPressed(QMouseEvent* event)
{
    //std::cout << "MOUSE BUTTON PRESSED " << event->button() << std::endl;

    // Manually grab focus for the viewport whenever a mouse button is pressed
    //widget->grab_focus();

    assert(QT_INPUT_MOUSE_BUTTON_MAP.find(event->button()) != QT_INPUT_MOUSE_BUTTON_MAP.end() && "Qt mouse button isn't registered with the engine.");

    MouseButtonCode mouseButton = QT_INPUT_MOUSE_BUTTON_MAP[event->button()];
    mouseButtonsDown[(size_t) mouseButton] = true;
}
void QtInputLayer::OnMouseButtonReleased(QMouseEvent* event)
{
    //std::cout << "MOUSE BUTTON RELEASED " << event->button() << std::endl;

    assert(QT_INPUT_MOUSE_BUTTON_MAP.find(event->button()) != QT_INPUT_MOUSE_BUTTON_MAP.end() && "Qt mouse button isn't registered with the engine.");

    MouseButtonCode mouseButton = QT_INPUT_MOUSE_BUTTON_MAP[event->button()];
    mouseButtonsDown[(size_t) mouseButton] = false;
}

bool QtInputLayer::OnKeyPressed(QKeyEvent* event)
{
    //std::cout << "KEY PRESSED " << event->key() << std::endl;

    assert(QT_INPUT_KEY_MAP.find(event->key()) != QT_INPUT_KEY_MAP.end() && "Qt key isn't registered with the engine.");

    RFKeyCode key = QT_INPUT_KEY_MAP[event->key()];
    keysDown[(size_t) key] = true;
    keysPressedThisFrame[(size_t) key] = true;

    return true;
}
void QtInputLayer::OnKeyReleased(QKeyEvent* event)
{
    //std::cout << "KEY RELEASED " << event->key() << std::endl;

    assert(QT_INPUT_KEY_MAP.find(event->key()) != QT_INPUT_KEY_MAP.end() && "Qt key isn't registered with the engine.");

    RFKeyCode key = QT_INPUT_KEY_MAP[event->key()];
    keysDown[(size_t) key] = false;
    keysReleasedThisFrame[(size_t) key] = true;
}