#pragma once

#include <filesystem>

#include <QStandardItemModel>
#include <QMimeData>
#include <QIODevice>

class FileItemModel : public QStandardItemModel
{
	Q_OBJECT

private:
	std::filesystem::path currentDirectory;

public:
	explicit FileItemModel(QObject* parent = nullptr);
	~FileItemModel();

    void SetCurrentDirectory(const std::filesystem::path& currentDirectory);

protected:
	QMimeData* mimeData(const QModelIndexList& indexes) const override;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;

    QIcon GetFileTypeIcon(const std::filesystem::path& path) const;

public:
	std::filesystem::path GetCurrentDirectory() { return currentDirectory; }
};