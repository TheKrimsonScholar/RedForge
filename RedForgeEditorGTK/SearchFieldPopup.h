//#pragma once
//
//#include <gtkmm/box.h>
//#include <gtkmm/listbox.h>
//#include <gtkmm/searchentry.h>
//#include <gtkmm/popover.h>
//#include <gtkmm/scrolledwindow.h>
//#include <gtkmm/eventcontrollerkey.h>
//#include <gtkmm/gestureclick.h>
//
//class SearchFieldPopup : public Gtk::Box
//{
//private:
//    // UI Components
//    Gtk::SearchEntry m_search_entry;
//    Gtk::Popover m_popover;
//    Gtk::ScrolledWindow m_scrolled_window;
//    Gtk::ListBox m_listbox;
//
//    // Data
//    std::vector<std::string> m_all_items;
//    std::vector<std::string> m_filtered_items;
//	std::unordered_map<std::string, uint32_t> m_item_map;
//
//    // Event controllers
//    Glib::RefPtr<Gtk::EventControllerKey> m_key_controller;
//    Glib::RefPtr<Gtk::GestureClick> m_click_gesture;
//
//    // Signal
//    sigc::signal<void(uint32_t, const std::string&)> m_signal_selection_made;
//
//    // State
//    bool m_programmatic_change = false;
//    bool m_popover_should_be_visible = false;
//
//public:
//    // Signal emitted when a selection is made
//    sigc::signal<void(uint32_t, const std::string&)> signal_selection_made() { return m_signal_selection_made; }
//
//public:
//    SearchFieldPopup(const Glib::ustring& placeholderText = "Search and select...");
//    ~SearchFieldPopup();
//
//    void SetSelectionList(std::vector<std::string> selections);
//
//private:
//    void on_search_changed();
//    void on_row_activated(Gtk::ListBoxRow* row);
//    void filter_results(const std::string& query);
//    void show_results();
//    void hide_results();
//    void select_item(uint32_t index, const std::string& item);
//    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
//    void on_button_pressed(int n, double x, double y);
//};