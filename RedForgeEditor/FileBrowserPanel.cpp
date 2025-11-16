#include "FileBrowserPanel.h"

#include "FileManager.h"

#include "DebugMacros.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

FileBrowserPanel::FileBrowserPanel(QWidget* parent) : EditorPanel("File Browser", parent)
{
    static const QIcon UP_ICON = QIcon(GetEditorAssetsPath().append("Icons/Basic/Up.png").string().c_str());
    static const QIcon REFRESH_ICON = QIcon(GetEditorAssetsPath().append("Icons/Basic/Refresh.png").string().c_str());

    listView = new FileListView(this);

    gameDirectoryButton = new QRadioButton("Game", this);
    QObject::connect(gameDirectoryButton, &QRadioButton::clicked,
        [this](bool checked)
        {
            listView->SetCurrentDirectory(GetGameAssetsPath());
        });
    engineDirectoryButton = new QRadioButton("Engine", this);
    QObject::connect(engineDirectoryButton, &QRadioButton::clicked,
        [this](bool checked)
        {
            listView->SetCurrentDirectory(GetEngineAssetsPath());
        });
    editorDirectoryButton = new QRadioButton("Editor", this);
    QObject::connect(editorDirectoryButton, &QRadioButton::clicked,
        [this](bool checked)
        {
            listView->SetCurrentDirectory(GetEditorAssetsPath());
        });

    QHBoxLayout* hBox1 = new QHBoxLayout();
    hBox1->addWidget(gameDirectoryButton);
    hBox1->addWidget(engineDirectoryButton);
    hBox1->addWidget(editorDirectoryButton);

    directoryUpButton = new QToolButton(this);
    directoryUpButton->setIcon(UP_ICON);
    QObject::connect(directoryUpButton, &QToolButton::clicked,
        [this](bool checked)
        {
            listView->SetCurrentDirectory(GetCurrentDirectory().parent_path());
        });
    directoryRefreshButton = new QToolButton(this);
    directoryRefreshButton->setIcon(REFRESH_ICON);
    QObject::connect(directoryRefreshButton, &QToolButton::clicked,
        [this](bool checked)
        {
            listView->SetCurrentDirectory(GetCurrentDirectory());
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

void FileBrowserPanel::Initialize()
{
    // Game directory is the default current directory
    gameDirectoryButton->click();
}
void FileBrowserPanel::Update()
{

}