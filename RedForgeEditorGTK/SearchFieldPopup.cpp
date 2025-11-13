//#include "SearchFieldPopup.h"
//
//#include "DebugMacros.h"
//
//#include "gtkmm/label.h"
//#include "glibmm.h"
//
//SearchFieldPopup::SearchFieldPopup(const Glib::ustring& placeholderText) : Gtk::Box(Gtk::Orientation::VERTICAL, 6)
//{
//    set_margin_top(12);
//    set_margin_bottom(12);
//    set_margin_start(12);
//    set_margin_end(12);
//
//    // Configure search entry
//    m_search_entry.set_placeholder_text(placeholderText);
//    m_search_entry.set_hexpand(true);
//
//    // Configure popover
//    m_popover.set_parent(m_search_entry);
//    m_popover.set_autohide(true);  // Allow it to hide when clicking outside
//    m_popover.set_has_arrow(false);
//    m_popover.set_position(Gtk::PositionType::BOTTOM);
//
//    // Configure scrolled window
//    m_scrolled_window.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
//    m_scrolled_window.set_min_content_height(200);
//    m_scrolled_window.set_max_content_height(300);
//    m_scrolled_window.set_min_content_width(300);
//
//    // Configure listbox
//    m_listbox.set_selection_mode(Gtk::SelectionMode::SINGLE);
//    m_listbox.add_css_class("boxed-list");
//
//    // Set up popover content
//    m_scrolled_window.set_child(m_listbox);
//    m_popover.set_child(m_scrolled_window);
//
//    // Add search entry to main container
//    append(m_search_entry);
//
//    // Set up click gesture for detecting clicks on search entry
//    m_click_gesture = Gtk::GestureClick::create();
//    m_click_gesture->signal_pressed().connect(
//        sigc::mem_fun(*this, &SearchFieldPopup::on_button_pressed));
//    m_search_entry.add_controller(m_click_gesture);
//
//    // Set up keyboard controller
//    m_key_controller = Gtk::EventControllerKey::create();
//    m_key_controller->signal_key_pressed().connect(
//        sigc::mem_fun(*this, &SearchFieldPopup::on_key_pressed), false);
//    m_search_entry.add_controller(m_key_controller);
//
//    // Connect signals
//    m_search_entry.signal_search_changed().connect(
//        sigc::mem_fun(*this, &SearchFieldPopup::on_search_changed));
//
//    m_search_entry.signal_activate().connect([this]() {
//        // Handle Enter key in search entry
//        auto selected = m_listbox.get_selected_row();
//        if (selected && m_popover.get_visible()) {
//            on_row_activated(selected);
//        }
//        });
//
//    m_listbox.signal_row_activated().connect(
//        sigc::mem_fun(*this, &SearchFieldPopup::on_row_activated));
//
//    // Handle popover closing
//    m_popover.signal_closed().connect([this]() {
//        m_popover_should_be_visible = false;
//        });
//}
//SearchFieldPopup::~SearchFieldPopup()
//{
//    m_popover.unparent();
//}
//
//void SearchFieldPopup::SetSelectionList(std::vector<std::string> selections)
//{
//    m_all_items = selections;
//
//	m_item_map.clear();
//	for(uint32_t i = 0; i < m_all_items.size(); i++)
//		m_item_map[m_all_items[i]] = i;
//}
//
//void SearchFieldPopup::on_button_pressed(int n, double x, double y)
//{
//    // User clicked on search entry - show results
//    std::string current_text = m_search_entry.get_text();
//    filter_results(current_text);
//    show_results();
//}
//
//void SearchFieldPopup::on_search_changed()
//{
//    if (!m_programmatic_change) {
//        std::string query = m_search_entry.get_text();
//        filter_results(query);
//
//        // If user is typing, show results
//        if (!query.empty()) {
//            show_results();
//        }
//        else {
//            // If they cleared the text, show all items
//            show_results();
//        }
//    }
//}
//
//bool SearchFieldPopup::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state)
//{
//    // Show results when user starts typing (if not already visible)
//    if (!m_popover.get_visible() &&
//        (keyval >= GDK_KEY_a && keyval <= GDK_KEY_z) ||
//        (keyval >= GDK_KEY_A && keyval <= GDK_KEY_Z) ||
//        (keyval >= GDK_KEY_0 && keyval <= GDK_KEY_9) ||
//        keyval == GDK_KEY_space || keyval == GDK_KEY_BackSpace) {
//
//        // User is typing - show results after the keystroke is processed
//        Glib::signal_idle().connect_once([this]()
//            {
//            if (!m_programmatic_change) {
//                filter_results(m_search_entry.get_text());
//                show_results();
//            }
//            });
//    }
//
//    if (!m_popover.get_visible()) {
//        return false;
//    }
//
//    switch (keyval) {
//    case GDK_KEY_Down:
//    case GDK_KEY_KP_Down: {
//        // Navigate down in the list
//        auto selected = m_listbox.get_selected_row();
//        if (selected) {
//            auto next = selected->get_next_sibling();
//            if (next) {
//                m_listbox.select_row(dynamic_cast<Gtk::ListBoxRow&>(*next));
//            }
//        }
//        else if (m_listbox.get_first_child()) {
//            m_listbox.select_row(dynamic_cast<Gtk::ListBoxRow&>(*m_listbox.get_first_child()));
//        }
//        return true;
//    }
//
//    case GDK_KEY_Up:
//    case GDK_KEY_KP_Up: {
//        // Navigate up in the list
//        auto selected = m_listbox.get_selected_row();
//        if (selected) {
//            auto prev = selected->get_prev_sibling();
//            if (prev) {
//                m_listbox.select_row(dynamic_cast<Gtk::ListBoxRow&>(*prev));
//            }
//        }
//        return true;
//    }
//
//    case GDK_KEY_Return:
//    case GDK_KEY_KP_Enter: {
//        // Select the currently highlighted item
//        auto selected = m_listbox.get_selected_row();
//        if (selected) {
//            on_row_activated(selected);
//            return true;
//        }
//        break;
//    }
//
//    case GDK_KEY_Escape: {
//        // Hide the list without selecting
//        hide_results();
//        return true;
//    }
//    }
//
//    return false;
//}
//
//void SearchFieldPopup::filter_results(const std::string& query)
//{
//    // Clear existing results
//    auto child = m_listbox.get_first_child();
//    while (child) {
//        auto next = child->get_next_sibling();
//        m_listbox.remove(*child);
//        child = next;
//    }
//
//    m_filtered_items.clear();
//
//    // Convert query to lowercase for case-insensitive search
//    std::string lower_query = query;
//    std::transform(lower_query.begin(), lower_query.end(),
//        lower_query.begin(), ::tolower);
//
//    // Filter and add matching items
//    for (const auto& item : m_all_items)
//    {
//        std::string lower_item = item;
//        std::transform(lower_item.begin(), lower_item.end(),
//            lower_item.begin(), ::tolower);
//
//        if (query.empty() || lower_item.find(lower_query) != std::string::npos)
//        {
//            auto label = new Gtk::Label(item);
//            label->set_halign(Gtk::Align::START);
//            label->set_margin_top(8);
//            label->set_margin_bottom(8);
//            label->set_margin_start(12);
//            label->set_margin_end(12);
//
//            m_listbox.append(*label);
//            m_filtered_items.push_back(item);
//        }
//    }
//
//    // Show message if no results
//    if (m_filtered_items.empty() && !query.empty())
//    {
//        auto no_results = new Gtk::Label("No results found");
//        no_results->set_halign(Gtk::Align::CENTER);
//        no_results->set_margin_top(20);
//        no_results->set_margin_bottom(20);
//        no_results->add_css_class("dim-label");
//        m_listbox.append(*no_results);
//    }
//}
//
//void SearchFieldPopup::show_results()
//{
//    m_popover_should_be_visible = true;
//    m_popover.popup();
//}
//
//void SearchFieldPopup::hide_results()
//{
//    m_popover_should_be_visible = false;
//    m_popover.popdown();
//}
//
//void SearchFieldPopup::on_row_activated(Gtk::ListBoxRow* row)
//{
//    if(!row)
//        return;
//
//    int index = row->get_index();
//    if(index >= 0 && index < static_cast<int>(m_filtered_items.size()))
//        select_item(m_item_map[m_filtered_items[index]], m_filtered_items[index]);
//}
//
//void SearchFieldPopup::select_item(uint32_t index, const std::string& item)
//{
//    // Hide the results first
//    hide_results();
//
//    // Clear the search entry text (reset after selection)
//    m_programmatic_change = true;
//    m_search_entry.set_text("");
//    m_programmatic_change = false;
//
//    // Emit signal with selected item
//    m_signal_selection_made.emit(index, item);
//
//    // Your custom logic here
//    g_print("Selected item: %s\n", item.c_str());
//    g_print("Performing action with: %s\n", item.c_str());
//}