#pragma once

#include "EditorPanel.h"

#include <filesystem>

#include <QToolButton>
#include <QRadioButton>

#include "FileListView.h"

class FileBrowserPanel : public EditorPanel
{
    Q_OBJECT

private:
    FileListView* listView;
    QRadioButton* gameDirectoryButton;
    QRadioButton* engineDirectoryButton;
    QRadioButton* editorDirectoryButton;
    QToolButton* directoryUpButton;
    QToolButton* directoryRefreshButton;

public:
    FileBrowserPanel(QWidget* parent = nullptr);
    ~FileBrowserPanel();

protected:
    void Initialize() override;
    void Update() override;

    friend class MainEditorWindow;

public:
    std::filesystem::path GetCurrentDirectory() { return listView->GetCurrentDirectory(); }
};