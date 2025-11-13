#include "FileItemModel.h"

#include "FileManager.h"

FileItemModel::FileItemModel(QObject* parent) : QStandardItemModel(parent)
{

}
FileItemModel::~FileItemModel()
{

}

void FileItemModel::SetCurrentDirectory(const std::filesystem::path& currentDirectory)
{
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

QIcon FileItemModel::GetFileTypeIcon(const std::filesystem::path& path) const
{
    static const QIcon INVALID_ICON = QIcon(GetEditorAssetsPath().append("Icons/File Browser/Invalid.png").string().c_str());
    static const QIcon FOLDER_ICON = QIcon(GetEditorAssetsPath().append("Icons/File Browser/Folder.png").string().c_str());
    static const QIcon TEXT_ICON = QIcon(GetEditorAssetsPath().append("Icons/File Browser/Text.png").string().c_str());
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
    
    // Default (unknown file type)
    return UNKNOWN_ICON;
}