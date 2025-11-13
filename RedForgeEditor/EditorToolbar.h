#pragma once

#include <QWidget>

#include <QPushButton>

class EditorToolbar : public QWidget
{
	Q_OBJECT

private:
	QPushButton* saveLevelButton;

public:
	explicit EditorToolbar(QWidget* parent = nullptr);
	~EditorToolbar();

private:
	void SaveLevel();
};