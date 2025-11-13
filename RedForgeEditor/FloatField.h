#pragma once

#include <QDoubleSpinBox>

class FloatField : public QDoubleSpinBox
{
	Q_OBJECT

private:
	bool m_dragging;
	QPoint m_dragStartPos;
	double m_valueOnDragStart;
	double m_pixelsPerUnit;  // how many pixels correspond to one unit change

public:
	FloatField(double initialValue = 0.0, int decimals = 3,
		double min = -std::numeric_limits<double>::max(), double max = std::numeric_limits<double>::max(), 
		double increment = 1.0, QWidget* parent = nullptr);
	~FloatField();

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
};