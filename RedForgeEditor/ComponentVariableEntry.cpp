#include "ComponentVariableEntry.h"

#include "EntityManager.h"

#include <QHBoxLayout>

ComponentVariableEntry::ComponentVariableEntry(const std::string& label, void* variablePtr, QWidget* parent) : QWidget(parent)
{
	variableLabel = new QLabel(label.c_str(), this);

	QHBoxLayout* hBox = new QHBoxLayout();
	hBox->addWidget(variableLabel);

	setLayout(hBox);
}
ComponentVariableEntry::~ComponentVariableEntry()
{
	
}