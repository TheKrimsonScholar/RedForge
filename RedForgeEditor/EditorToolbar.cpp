#include "EditorToolbar.h"

#include "LevelManager.h"

#include <QHBoxLayout>

#include "EditorPaths.h"

EditorToolbar::EditorToolbar(QWidget* parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	saveLevelButton = new QPushButton(QICON_FROM_PATH("Save"), "", this);
	QObject::connect(saveLevelButton, &QPushButton::clicked, this, &EditorToolbar::SaveLevel);

	QHBoxLayout* hBox = new QHBoxLayout();
	hBox->addWidget(saveLevelButton);
	setLayout(hBox);
}
EditorToolbar::~EditorToolbar()
{

}

void EditorToolbar::SaveLevel()
{
	LevelManager::SaveLevel();
}