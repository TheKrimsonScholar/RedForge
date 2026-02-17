#pragma once

#include <queue>

#include "InputLayer.h"

#include <QWidget>
#include <QWindow>

class QtInputLayer : public InputLayer, public QObject
{
private:
	QWindow* window;

	std::queue<std::function<void(InputState&)>> inputEvents;

public:
	QtInputLayer(QWindow* window);
	~QtInputLayer() override;

	void Startup(const EngineStartupParams& params, InputState& inputState) override;
	void Shutdown(const EngineShutdownParams& params, InputState& inputState) override;

	void PreUpdate(InputState& inputState) override;
	void PostUpdate(InputState& inputState) override;

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

	void OnMouseMove(InputState& inputState, const QPointF& position);

	void OnMouseButtonPressed(InputState& inputState, Qt::MouseButton button);
	void OnMouseButtonReleased(InputState& inputState, Qt::MouseButton button);

	bool OnKeyPressed(InputState& inputState, int key);
	void OnKeyReleased(InputState& inputState, int key);
};