#pragma once

#include "InputLayer.h"

#include <QWidget>
#include <QWindow>

class QtInputLayer : public InputLayer, public QObject
{
private:
	QWindow* window;

public:
	QtInputLayer(QWindow* window);
	~QtInputLayer() override;

	void PreUpdate() override;
	void PostUpdate() override;

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

	void OnMouseMove(QMouseEvent* event);

	void OnMouseButtonPressed(QMouseEvent* event);
	void OnMouseButtonReleased(QMouseEvent* event);

	bool OnKeyPressed(QKeyEvent* event);
	void OnKeyReleased(QKeyEvent* event);
};