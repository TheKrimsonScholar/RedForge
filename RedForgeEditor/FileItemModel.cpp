#include "FileItemModel.h"

#include "FileManager.h"
#include "LevelManager.h"

#include <QMimeData>
#include <QDropEvent>

FileItemModel::FileItemModel(QObject* parent) : QStandardItemModel(parent)
{

}
FileItemModel::~FileItemModel()
{

}

void FileItemModel::SetCurrentDirectory(const std::filesystem::path& currentDirectory)
{
    this->currentDirectory = currentDirectory;

	clear();

    for(const std::filesystem::path& file : FileManager::GetAllTopLevelItemsInDirectory(currentDirectory))
    {
        std::filesystem::path absolutePath = currentDirectory;
        absolutePath.append(file.string());

        QStandardItem* item = new QStandardItem(GetFileTypeIcon(absolutePath), file.string().c_str());
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
        item->setData(QVariant::fromValue(QString(absolutePath.string().c_str())), Qt::UserRole);

        appendRow(item);
    }
}

QMimeData* FileItemModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mimeData = QStandardItemModel::mimeData(indexes);
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    for(const QModelIndex& index : indexes)
    {
        if(index.isValid())
        {
            QMap<int, QVariant> roleDataMap;
            roleDataMap.insert(Qt::DisplayRole, data(index, Qt::DisplayRole));
            roleDataMap.insert(Qt::UserRole, data(index, Qt::UserRole));

            stream << index.row() << index.column() << roleDataMap;
        }
    }

    mimeData->setData("application/x-filelistitem", encoded);
    return mimeData;
}
bool FileItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{	
	// Only accept entity drops (for creating prefab assets)
	if(data->hasFormat("application/x-entityhierarchyitem"))
	{
		QByteArray encodedData = data->data("application/x-entityhierarchyitem");
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		QVariant itemData;
			
		while(!stream.atEnd())
		{
			int r, c;
			QMap<int, QVariant> roleDataMap;
			stream >> r >> c >> roleDataMap;

			Entity entity = roleDataMap[Qt::UserRole].value<Entity>();
			qDebug() << "Dropped Item Data:" << r << c << roleDataMap[Qt::DisplayRole] << LevelManager::GetEntityName(entity);

			std::filesystem::path prefabPath = currentDirectory;
			prefabPath.append(LevelManager::GetEntityName(entity)).replace_extension(".entity");
			LevelManager::SaveEntityAsPrefab(entity, prefabPath);

			// Refresh view
			SetCurrentDirectory(GetCurrentDirectory());
		}
	}

    return false;
}

QIcon FileItemModel::GetFileTypeIcon(const std::filesystem::path& path) const
{
    static const QIcon INVALID_ICON = QIcon(GetEditorAssetsPath().append("Icons/File Browser/Invalid.png").string().c_str());
    static const QIcon FOLDER_ICON = QIcon(GetEditorAssetsPath().append("Icons/File Browser/Folder.png").string().c_str());
    static const QIcon TEXT_ICON = QIcon(GetEditorAssetsPath().append("Icons/File Browser/Text.png").string().c_str());
    static const QIcon PREFAB_ICON = QIcon(GetEditorAssetsPath().append("Icons/Entity Hierarchy/Prefab.png").string().c_str());
    static const QIcon UNKNOWN_ICON = QIcon(GetEditorAssetsPath().append("Icons/File Browser/Unknown.png").string().c_str());

    if(!std::filesystem::exists(path))
        return INVALID_ICON;

    // Folder
    if(std::filesystem::is_directory(path))
        return FOLDER_ICON;

    // Text
    if(path.extension().string() == ".txt")
        return TEXT_ICON;
    // Image - use the image itself
    if(path.extension().string() == ".png" || path.extension().string() == ".ico")
        return QIcon(path.string().c_str());
    // Prefab
    if(path.extension().string() == ".entity")
        return PREFAB_ICON;
    
    // Default (unknown file type)
    return UNKNOWN_ICON;
}