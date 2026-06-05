#include "QtInputLayer.h"

#include <cassert>
#include <iostream>

#include <QMouseEvent>
#include <QKeyEvent>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

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

void QtInputLayer::Startup(const EngineStartupParams& params, InputState& inputState)
{

}
void QtInputLayer::Shutdown(const EngineShutdownParams& params, InputState& inputState)
{

}

void QtInputLayer::PreUpdate(InputState& inputState)
{
    // Call all input events that were queued since last frame
    while(!inputEvents.empty())
    {
        inputEvents.front()(inputState);
        inputEvents.pop();
    }

    InputLayer::PreUpdate(inputState);
}
void QtInputLayer::PostUpdate(InputState& inputState)
{
    InputLayer::PostUpdate(inputState);
}

bool QtInputLayer::eventFilter(QObject* watched, QEvent* event)
{
    QEvent::Type eType = event->type();
    switch(event->type())
    {
        case QEvent::MouseMove:
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF position = mouseEvent->position();

            inputEvents.push([this, position](InputState& inputState)
                {
                    OnMouseMove(inputState, position);
                });
        }
        break;
        case QEvent::MouseButtonPress:
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			Qt::MouseButton button = mouseEvent->button();

            inputEvents.push([this, button](InputState& inputState)
                {
                    OnMouseButtonPressed(inputState, button);
                });
        }
        break;
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			Qt::MouseButton button = mouseEvent->button();

            inputEvents.push([this, button](InputState& inputState)
                {
                    OnMouseButtonReleased(inputState, button);
                });
        }
        break;
        case QEvent::KeyPress:
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            int key = keyEvent->key();
            
            inputEvents.push([this, key](InputState& inputState)
                {
                    OnKeyPressed(inputState, key);
                });
        }
        break;
        case QEvent::KeyRelease:
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            int key = keyEvent->key();
            
            inputEvents.push([this, key](InputState& inputState)
                {
                    OnKeyReleased(inputState, key);
                });
        }
        break;
        default: break;
    }

    return false;
}

void QtInputLayer::OnMouseMove(InputState& inputState, const QPointF& position)
{
    // Convert from device-independent pixel coordinates to physical pixel coordinates
    inputState.mousePosition = { position.x() * window->devicePixelRatio(), position.y() * window->devicePixelRatio() };
}

void QtInputLayer::OnMouseButtonPressed(InputState& inputState, Qt::MouseButton button)
{
    // Manually grab focus for the viewport whenever a mouse button is pressed
    //widget->grab_focus();

    assert(QT_INPUT_MOUSE_BUTTON_MAP.find(button) != QT_INPUT_MOUSE_BUTTON_MAP.end() && "Qt mouse button isn't registered with the engine.");

    MouseButtonCode mouseButton = QT_INPUT_MOUSE_BUTTON_MAP[button];
    inputState.mouseButtonsDown[(size_t) mouseButton] = true;
}
void QtInputLayer::OnMouseButtonReleased(InputState& inputState, Qt::MouseButton button)
{
    assert(QT_INPUT_MOUSE_BUTTON_MAP.find(button) != QT_INPUT_MOUSE_BUTTON_MAP.end() && "Qt mouse button isn't registered with the engine.");

    MouseButtonCode mouseButton = QT_INPUT_MOUSE_BUTTON_MAP[button];
    inputState.mouseButtonsDown[(size_t) mouseButton] = false;
}

bool QtInputLayer::OnKeyPressed(InputState& inputState, int key)
{
    assert(QT_INPUT_KEY_MAP.find(key) != QT_INPUT_KEY_MAP.end() && "Qt key isn't registered with the engine.");

    RFKeyCode keyPressed = QT_INPUT_KEY_MAP[key];
    inputState.keysDown[(size_t) keyPressed] = true;
    inputState.keysPressedThisFrame[(size_t)keyPressed] = true;

    return true;
}
void QtInputLayer::OnKeyReleased(InputState& inputState, int key)
{
    assert(QT_INPUT_KEY_MAP.find(key) != QT_INPUT_KEY_MAP.end() && "Qt key isn't registered with the engine.");

    RFKeyCode keyReleased = QT_INPUT_KEY_MAP[key];
    inputState.keysDown[(size_t) keyReleased] = false;
    inputState.keysReleasedThisFrame[(size_t) keyReleased] = true;
}