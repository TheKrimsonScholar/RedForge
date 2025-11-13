#pragma once

#include <QLineEdit>
#include <QMouseEvent>

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
    explicit DragFloat(float initialValue, QWidget* parent = nullptr);
    ~DragFloat();

protected:
    void focusInEvent(QFocusEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

public:
    float GetValue() { return value; }

    void SetValue(float value);

signals:
    void valueChanged(float value);
};