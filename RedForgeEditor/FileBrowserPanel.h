#pragma once

#include "EditorPanel.h"

#include <filesystem>

#include <QListView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QRadioButton>

class FileBrowserPanel : public EditorPanel
{
    Q_OBJECT

private:
    QListView* listView;
    QStandardItemModel* model;
    QRadioButton* gameDirectoryButton;
    QRadioButton* engineDirectoryButton;
    QRadioButton* editorDirectoryButton;
    QPushButton* directoryUpButton;
    QPushButton* directoryRefreshButton;

    std::filesystem::path currentDirectory;

public:
    FileBrowserPanel(QWidget* parent = nullptr);
    ~FileBrowserPanel();

    void SetCurrentBrowserDirectory(const std::filesystem::path& currentDirectory);

protected:
    void Initialize() override;
    void Update() override;

    friend class MainEditorWindow;

private:
    QIcon GetFileTypeIcon(const std::filesystem::path& path) const;
};