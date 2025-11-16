#pragma once

#include <QWidget>

#include <QToolButton>

class EditorToolbar : public QWidget
{
	Q_OBJECT

private:
	QToolButton* saveLevelButton;
	QToolButton* playButton;

public:
	explicit EditorToolbar(QWidget* parent = nullptr);
	~EditorToolbar();

private:
	void SaveLevel();
	
	void Play();
};