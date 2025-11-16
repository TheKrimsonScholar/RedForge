#include "FileListView.h"

FileListView::FileListView(QWidget* parent) : QListView(parent)
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	setDefaultDropAction(Qt::CopyAction);
	setAcceptDrops(true);
	
	model = new FileItemModel(this);
	setModel(model);

	QObject::connect(this, &FileListView::doubleClicked, 
        [this](const QModelIndex& index)
        {
            QStandardItem* item = model->itemFromIndex(index);
            if(!item)
                return;

            QString text = item->text();
            qDebug() << "Clicked item:" << text;

            std::filesystem::path absolutePath = GetCurrentDirectory();
            absolutePath.append(text.toStdString());

            // If folder, update current directory
            if(std::filesystem::is_directory(absolutePath))
                SetCurrentDirectory(absolutePath);
        });
}
FileListView::~FileListView()
{

}