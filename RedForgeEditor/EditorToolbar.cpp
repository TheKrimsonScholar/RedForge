#include "EditorToolbar.h"

#include "LevelManager.h"
#include "PhysicsSystem.h"

#include <QHBoxLayout>

#include "EditorPaths.h"

EditorToolbar::EditorToolbar(QWidget* parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	saveLevelButton = new QToolButton(this);
	saveLevelButton->setIcon(QICON_FROM_PATH("Toolbar/Save"));
	QObject::connect(saveLevelButton, &QToolButton::clicked, this, &EditorToolbar::SaveLevel);

	playButton = new QToolButton(this);
	playButton->setIcon(QICON_FROM_PATH("Toolbar/Play"));
	QObject::connect(playButton, &QToolButton::clicked, this, &EditorToolbar::Play);

	QHBoxLayout* hBox = new QHBoxLayout();
	hBox->addWidget(saveLevelButton);
	hBox->addWidget(playButton);
	setLayout(hBox);
}
EditorToolbar::~EditorToolbar()
{

}

void EditorToolbar::SaveLevel()
{
	LevelManager::SaveLevel(L"Levels/Level.txt");
}

void EditorToolbar::Play()
{
	PhysicsSystem::Unpause();
}