#include "FloatField.h"

#include <QMouseEvent>
#include <QLineEdit>
#include <QApplication>

FloatField::FloatField(double initialValue, int decimals, double min, double max, double increment, QWidget* parent) : QDoubleSpinBox(parent)
{
	setValue(initialValue);
	setDecimals(decimals);
	setRange(min, max);
	setSingleStep(increment);
	setAccelerated(true);

    lineEdit()->installEventFilter(this);
}
FloatField::~FloatField()
{

}

void FloatField::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "MOUSE PRESS";

    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->pos();
        m_valueOnDragStart = value();
        m_dragging = true;
        // optionally change cursor
        setCursor(Qt::SizeHorCursor);
        // eat the event
        event->accept();
        return;
    }
    // otherwise default
    QDoubleSpinBox::mousePressEvent(event);
}

void FloatField::mouseMoveEvent(QMouseEvent* event)
{
    qDebug() << "MOUSE MOVE";

    if (m_dragging) {
        int dx = event->pos().x() - m_dragStartPos.x();
        // convert dx -> deltaValue
        double delta = dx / m_pixelsPerUnit;
        double newVal = m_valueOnDragStart + delta;
        // clamp to min/max
        if (newVal < minimum()) newVal = minimum();
        if (newVal > maximum()) newVal = maximum();
        // set the new value
        setValue(newVal);
        event->accept();
        return;
    }
    QDoubleSpinBox::mouseMoveEvent(event);
}

void FloatField::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug() << "MOUSE RELEASE";

    if (m_dragging && event->button() == Qt::LeftButton) {
        m_dragging = false;
        // restore normal cursor
        unsetCursor();
        event->accept();
        return;
    }
    QDoubleSpinBox::mouseReleaseEvent(event);
}