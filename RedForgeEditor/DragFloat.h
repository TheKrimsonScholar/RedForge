#pragma once

#include <QLineEdit>
#include <QMouseEvent>
#include <QDoubleValidator>

class DragFloat : public QLineEdit
{
    Q_OBJECT

private:
    int precision = 2;
    float increment = 0.1f;
    int pixelsPerIncrement = 5;

    bool isPressed = false;
    bool isDragging = false;
    QPoint pressPosition = QPoint(0, 0);
    float pressValue = 0.0f;
    
    float value = 0.0f;

public:
    explicit DragFloat(float initialValue, QWidget* parent = nullptr) : QLineEdit(parent)
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

protected:
    void focusInEvent(QFocusEvent* event) override
    {
        // Highlight the full value upon focusing in (for quick edits)
        selectAll();

        QLineEdit::focusInEvent(event);
    }

    void mousePressEvent(QMouseEvent* event) override
    {
        isPressed = true;

        pressPosition = event->pos();
        pressValue = value;

        QLineEdit::mousePressEvent(event);
    }
    void mouseMoveEvent(QMouseEvent* event) override
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
    void mouseReleaseEvent(QMouseEvent* event) override
    {
        isPressed = false;

        // Only focus in if not dragging value
        if(!isDragging)
            setFocus(Qt::OtherFocusReason);
        
        isDragging = false;
    }

public:
    float GetValue() { return value; }

    void SetValue(float value)
    {
        this->value = value;
        setText(QString::number(value, 'f', precision));

        emit valueChanged(value);
    }

signals:
    void valueChanged(float value);
};