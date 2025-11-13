//#include "EditorToolbar.h"
//
//#include "LevelManager.h"
//
//#include "DebugMacros.h"
//
//#include "gtkmm/image.h"
//
//EditorToolbar::EditorToolbar() : Gtk::Box(Gtk::Orientation::HORIZONTAL), 
//	saveButton()
//{
//	add_css_class("toolbar");
//
//	saveButton.add_css_class("toolbar-button");
//	saveButton.set_tooltip_text("Save the current level");
//	saveButton.signal_clicked().connect(sigc::mem_fun(*this, &EditorToolbar::SaveLevel), false);
//	Gtk::Image* saveIcon = Gtk::make_managed<Gtk::Image>();
//	Glib::RefPtr<Gdk::Texture> texture = Gdk::Texture::create_from_filename(GetEditorAssetsPath().append(L"Icons/Save.png").string());
//	saveIcon->set(texture);
//	saveButton.set_child(*saveIcon);
//
//	append(saveButton);
//}
//EditorToolbar::~EditorToolbar()
//{
//
//}
//
//void EditorToolbar::SaveLevel()
//{
//	LevelManager::SaveLevel();
//
//	LOG("[Level Saved]");
//}