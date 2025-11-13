#include "FileBrowserPanel.h"

#include "FileManager.h"

#include "DebugMacros.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

FileBrowserPanel::FileBrowserPanel(QWidget* parent) : EditorPanel("File Browser", parent)
{
    static const QIcon UP_ICON = QIcon(GetEditorAssetsPath().append("Icons/Basic/Up.png").string().c_str());
    static const QIcon REFRESH_ICON = QIcon(GetEditorAssetsPath().append("Icons/Basic/Refresh.png").string().c_str());

    listView = new QListView(this);
    listView->setDragEnabled(true);

    model = new FileItemModel(this);
    listView->setModel(model);

    listView->setModel(model);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listView->setDragDropMode(QAbstractItemView::DragDrop);
    listView->setDefaultDropAction(Qt::CopyAction);

    QObject::connect(listView, &QListView::doubleClicked, 
        [this](const QModelIndex& index)
        {
            QStandardItem* item = model->itemFromIndex(index);
            if(!item)
                return;

            QString text = item->text();
            qDebug() << "Clicked item:" << text;

            std::filesystem::path absolutePath = currentDirectory;
            absolutePath.append(text.toStdString());

            // If folder, update current directory
            if(std::filesystem::is_directory(absolutePath))
                SetCurrentBrowserDirectory(absolutePath);
        });

    gameDirectoryButton = new QRadioButton("Game", this);
    QObject::connect(gameDirectoryButton, &QRadioButton::clicked,
        [this](bool checked)
        {
            SetCurrentBrowserDirectory(GetGameAssetsPath());
        });
    engineDirectoryButton = new QRadioButton("Engine", this);
    QObject::connect(engineDirectoryButton, &QRadioButton::clicked,
        [this](bool checked)
        {
            SetCurrentBrowserDirectory(GetEngineAssetsPath());
        });
    editorDirectoryButton = new QRadioButton("Editor", this);
    QObject::connect(editorDirectoryButton, &QRadioButton::clicked,
        [this](bool checked)
        {
            SetCurrentBrowserDirectory(GetEditorAssetsPath());
        });

    QHBoxLayout* hBox1 = new QHBoxLayout();
    hBox1->addWidget(gameDirectoryButton);
    hBox1->addWidget(engineDirectoryButton);
    hBox1->addWidget(editorDirectoryButton);

    directoryUpButton = new QPushButton(UP_ICON, "", this);
    QObject::connect(directoryUpButton, &QPushButton::clicked,
        [this](bool checked)
        {
            SetCurrentBrowserDirectory(currentDirectory.parent_path());
        });
    directoryRefreshButton = new QPushButton(REFRESH_ICON, "", this);
    QObject::connect(directoryRefreshButton, &QPushButton::clicked,
        [this](bool checked)
        {
            SetCurrentBrowserDirectory(currentDirectory);
        });

    QHBoxLayout* hBox2 = new QHBoxLayout();
    hBox2->addWidget(directoryUpButton);
    hBox2->addWidget(directoryRefreshButton);
    hBox2->addStretch(5);

    QVBoxLayout* vBox = new QVBoxLayout();
    vBox->addLayout(hBox1);
    vBox->addLayout(hBox2);
    vBox->addWidget(listView);

    contentArea->setLayout(vBox);
}
FileBrowserPanel::~FileBrowserPanel()
{

}

void FileBrowserPanel::SetCurrentBrowserDirectory(const std::filesystem::path& currentDirectory)
{
    this->currentDirectory = currentDirectory;

    model->SetCurrentDirectory(currentDirectory);
}

void FileBrowserPanel::Initialize()
{
    // Game directory is the default current directory
    gameDirectoryButton->click();
}
void FileBrowserPanel::Update()
{

}