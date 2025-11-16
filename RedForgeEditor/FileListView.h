#pragma once

#include <filesystem>

#include <QListView>

#include "FileItemModel.h"

class FileListView : public QListView
{
	Q_OBJECT

private:
	FileItemModel* model;

public:
	explicit FileListView(QWidget* parent = nullptr);
	~FileListView();

	void SetCurrentDirectory(const std::filesystem::path& currentDirectory) { model->SetCurrentDirectory(currentDirectory); }

public:
	std::filesystem::path GetCurrentDirectory() { return model->GetCurrentDirectory(); }
};