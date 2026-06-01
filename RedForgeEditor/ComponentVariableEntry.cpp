#include "ComponentVariableEntry.h"

#include "EntityManager.h"

#include <QHBoxLayout>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

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