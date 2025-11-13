//#include "FileBrowserPanel.h"
//
//#include "FileManager.h"
//
//#include <gtkmm/image.h>
//#include <gtkmm/listview.h>
//#include <gtkmm/singleselection.h>
//#include <gtkmm/multiselection.h>
//#include <gtkmm/signallistitemfactory.h>
//#include <gtkmm/dragsource.h>
//
//#include <giomm.h>
//
//#include <iostream>
//
//FileBrowserPanel::FileBrowserPanel() : EditorPanel("File Browser"), 
//    m_ListView()
//{
//    contentArea.append(m_ListView);
//
//    listStore = Gio::ListStore<FileEntry>::create();
//    m_ListView.set_model(Gtk::MultiSelection::create(listStore));
//    m_ListView.add_css_class("data-table");
//
//    auto factory = Gtk::SignalListItemFactory::create();
//    factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& item)
//        {
//            auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
//            auto image = Gtk::make_managed<Gtk::Image>();
//            auto label = Gtk::make_managed<Gtk::Label>();
//            box->append(*image);
//            box->append(*label);
//            item->set_child(*box);
//
//            auto gesture = Gtk::GestureClick::create();
//            gesture->set_button(GDK_BUTTON_PRIMARY);
//            box->add_controller(gesture);
//
//            item->set_data("click-gesture", gesture.get());
//            item->set_data("click-connection", new sigc::connection());
//        });
//    factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& item)
//        {
//            auto row = std::dynamic_pointer_cast<FileEntry>(item->get_item());
//            auto box = dynamic_cast<Gtk::Box*>(item->get_child());
//            auto image = dynamic_cast<Gtk::Image*>(box->get_first_child());
//            auto label = dynamic_cast<Gtk::Label*>(box->get_last_child());
//
//            image->set(row->m_icon);
//            label->set_text(row->m_label);
//
//            // Connect click gesture to intercept expand/collapse clicks
//            auto gesture = static_cast<Gtk::GestureClick*>(item->get_data("click-gesture"));
//            auto connection = reinterpret_cast<sigc::connection*>(item->get_data("click-connection"));
//
//            // If a connection already exists, replace it
//            if(connection && connection->connected())
//                connection->disconnect();
//            if(gesture && connection)
//            {
//                // Override the current connection
//                *connection = gesture->signal_pressed().connect([this, row](int, double, double)
//                    {
//                        std::cout << "CLICKED " << row->m_label << std::endl;
//                    });
//            }
//
//            auto drag_source = Gtk::DragSource::create();
//            drag_source->set_actions(Gdk::DragAction::COPY);
//            drag_source->signal_prepare().connect([this, row](double x, double y) -> Glib::RefPtr<Gdk::ContentProvider>
//                {
//                    std::cout << "START DRAG " << row->m_label << std::endl;
//
//                    Glib::Value<Glib::ustring> value;
//                    value.init(Glib::Value<Glib::ustring>::value_type());
//
//                    std::filesystem::path directory = currentDirectory;
//                    value.set(directory.append("test").string());
//                    return Gdk::ContentProvider::create(value);
//                }, false);
//            box->add_controller(drag_source);
//        });
//    factory->signal_teardown().connect([](const Glib::RefPtr<Gtk::ListItem>& item)
//        {
//            std::cout << "TEARDOWN" << std::endl;
//
//            auto connection = reinterpret_cast<sigc::connection*>(item->get_data("click-connection"));
//            if(connection)
//            {
//                if(connection->connected())
//                    connection->disconnect();
//                delete connection;
//
//                item->set_data("click-connection", nullptr);
//            }
//        });
//    m_ListView.set_factory(factory);
//
//    //contentArea.append(fileList);
//
//    SetCurrentBrowserDirectory(GetGameAssetsPath());
//}
//FileBrowserPanel::~FileBrowserPanel()
//{
//
//}
//
//void FileBrowserPanel::SetCurrentBrowserDirectory(std::filesystem::path currentDirectory)
//{
//    //listStore->remove_all();
//
//    listStore->append(FileEntry::create("Cube", Gdk::Texture::create_from_filename(GetEngineAssetsPath().append("textures/texture.jpg").string()), false));
//    listStore->append(FileEntry::create("Sphere", Gdk::Texture::create_from_filename(GetEngineAssetsPath().append("textures/texture.jpg").string()), false));
//}
//
////void FileBrowserPanel::Initialize()
////{
////    //auto listView = Gtk::make_managed<Gtk::ListView>();
////
////    //auto store = Gio::ListStore<FileEntry>::create();
////    //store->append(FileEntry::create("Cube", Gdk::Texture::create_from_filename(GetEngineAssetsPath().append("textures/texture.jpg").string()), false));
////    //store->append(FileEntry::create("Sphere", Gdk::Texture::create_from_filename(GetEngineAssetsPath().append("textures/texture.jpg").string()), false));
////    //
////    ////auto selection_model = Gtk::MultiSelection::create(store);
////    ////auto selection_model = Gtk::MultiSelection::create(listModel);
////    //listView->set_model(Gtk::MultiSelection::create(store));
////
////    //auto factory = Gtk::SignalListItemFactory::create();
////    ///*factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& item)
////    //    {
////    //        auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
////    //        auto image = Gtk::make_managed<Gtk::Image>();
////    //        auto label = Gtk::make_managed<Gtk::Label>();
////    //        box->append(*image);
////    //        box->append(*label);
////    //        item->set_child(*box);
////    //    });
////    //factory->signal_bind().connect([this](const Glib::RefPtr<Gtk::ListItem>& item)
////    //    {
////    //        auto row = std::dynamic_pointer_cast<FileEntry>(item->get_item());
////    //        auto box = dynamic_cast<Gtk::Box*>(item->get_child());
////    //        auto image = dynamic_cast<Gtk::Image*>(box->get_first_child());
////    //        auto label = dynamic_cast<Gtk::Label*>(box->get_last_child());
////
////    //        image->set(row->m_icon);
////    //        label->set_text(row->m_label);
////
////    //        auto drag_source = Gtk::DragSource::create();
////    //        drag_source->set_actions(Gdk::DragAction::COPY);
////    //        drag_source->signal_prepare().connect([this, row](double x, double y) -> Glib::RefPtr<Gdk::ContentProvider>
////    //            {
////    //                Glib::Value<Glib::ustring> value;
////    //                value.init(Glib::Value<Glib::ustring>::value_type());
////
////    //                std::filesystem::path directory = currentDirectory;
////    //                value.set(directory.append("p").string());
////    //                return Gdk::ContentProvider::create(value);
////    //            }, false);
////    //        box->add_controller(drag_source);
////    //    });*/
////    //listView->set_factory(factory);
////
////    //fileList.set_child(*listView);
////}