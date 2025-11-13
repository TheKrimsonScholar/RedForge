#pragma once

#include <filesystem>

#include <QStandardItemModel>
#include <QMimeData>
#include <QIODevice>

class FileItemModel : public QStandardItemModel
{
	Q_OBJECT

public:
	explicit FileItemModel(QObject* parent = nullptr);
	~FileItemModel();

    void SetCurrentDirectory(const std::filesystem::path& currentDirectory);

protected:
    QMimeData* mimeData(const QModelIndexList& indexes) const
    {
        QMimeData* mimeData = new QMimeData();
        QByteArray encoded;
        QDataStream stream(&encoded, QIODevice::WriteOnly);

        for(const QModelIndex& index : indexes)
        {
            if(index.isValid())
            {
                stream << data(index, Qt::DisplayRole).toString();
                stream << data(index, Qt::UserRole).toString();
            }
        }

        mimeData->setData("application/x-filelistitem", encoded);
        return mimeData;
    }

    QIcon GetFileTypeIcon(const std::filesystem::path& path) const;
};