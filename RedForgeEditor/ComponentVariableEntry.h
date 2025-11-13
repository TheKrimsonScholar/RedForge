#pragma once

#include <typeindex>

#include <QWidget>
#include <QLabel>

class ComponentVariableEntry : public QWidget
{
	Q_OBJECT

protected:
	QLabel* variableLabel;

public:
	ComponentVariableEntry(const std::string& label, void* variablePtr, QWidget* parent = nullptr);
	~ComponentVariableEntry();

	virtual void UpdateDisplayedValue() = 0;
	virtual void OnValueChanged() = 0;
};