#pragma once

#include "EditorPanel.h"

#include <filesystem>

#include <gtkmm/listview.h>
#include <gtkmm/stringlist.h>
#include <glibmm/object.h>
#include <glibmm/property.h>
#include <glibmm/value.h>

#include <giomm/liststore.h>

class FileEntry : public Glib::Object
{
public:
    Glib::ustring m_label;
    Glib::RefPtr<Gdk::Paintable> m_icon;
    bool isFolder;

    static Glib::RefPtr<FileEntry> create(const Glib::ustring& label, const Glib::RefPtr<Gdk::Paintable>& icon, bool isFolder)
    {
        return Glib::make_refptr_for_instance<FileEntry>(new FileEntry(label, icon, isFolder));
    }

protected:
    FileEntry(const Glib::ustring& label, const Glib::RefPtr<Gdk::Paintable>& icon, bool isFolder) : Glib::Object(), 
        m_label(label), m_icon(icon), isFolder(isFolder)
    {

    }
};

class FileBrowserPanel : public EditorPanel
{
private:
    std::filesystem::path currentDirectory;

    Glib::RefPtr<Gio::ListStore<FileEntry>> listStore;

    Gtk::ListView m_ListView;

public:
	FileBrowserPanel();
	~FileBrowserPanel();

    void SetCurrentBrowserDirectory(std::filesystem::path currentDirectory);
};