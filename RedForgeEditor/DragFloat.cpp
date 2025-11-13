#include "DragFloat.h"

#include <QDoubleValidator>

DragFloat::DragFloat(float initialValue, QWidget* parent) : QLineEdit(parent)
{
    // Handle focus manually
    setFocusPolicy(Qt::NoFocus);
    setAlignment(Qt::AlignRight);

    // Only allow numeric input
    setValidator(new QDoubleValidator(-FLT_MAX, FLT_MAX, 6, this));

    QObject::connect(this, &DragFloat::editingFinished,
        [this]()
        {
            SetValue(text().toFloat());
        });

    // Initialize value
    SetValue(initialValue);
}
DragFloat::~DragFloat()
{

}

void DragFloat::focusInEvent(QFocusEvent* event)
{
    // Highlight the full value upon focusing in (for quick edits)
    selectAll();

    QLineEdit::focusInEvent(event);
}

void DragFloat::mousePressEvent(QMouseEvent* event)
{
    isPressed = true;

    pressPosition = event->pos();
    pressValue = value;

    QLineEdit::mousePressEvent(event);
}
void DragFloat::mouseMoveEvent(QMouseEvent* event)
{
    // Unfocused state: holding and dragging increments/decrements value
    if(!hasFocus())
    {
        setCursor(Qt::SizeHorCursor);

        if(isPressed)
        {
            isDragging = true;

            QPoint delta = event->pos() - pressPosition;
            SetValue(pressValue + increment * (delta.x() / pixelsPerIncrement));
        }
    }
    // Focused state: manual number input field; QLineEdit handles everything normally
    else
    {
        setCursor(Qt::IBeamCursor);

        QLineEdit::mouseMoveEvent(event);
    }
}
void DragFloat::mouseReleaseEvent(QMouseEvent* event)
{
    isPressed = false;

    // Only focus in if not dragging value
    if(!isDragging)
        setFocus(Qt::OtherFocusReason);
        
    isDragging = false;
}

void DragFloat::SetValue(float value)
{
    this->value = value;
    setText(QString::number(value, 'f', precision));

    emit valueChanged(value);
}