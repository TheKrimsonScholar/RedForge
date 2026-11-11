#pragma once

#include <typeindex>
#include <math.h>
#include <cmath>

#include "Event.h"

#include "Editor.h"

#include <QWidget>
#include <QLabel>

class ComponentVariableEntry : public QWidget
{
	Q_OBJECT

public:
	Event<void*> onValueChanged;

protected:
	QLabel* variableLabel;

public:
	ComponentVariableEntry(const std::string& label, void* variablePtr, QWidget* parent = nullptr);
	~ComponentVariableEntry();

	virtual void UpdateDisplayedValue() = 0;
	virtual void OnValueChanged() = 0;
};