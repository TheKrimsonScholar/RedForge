//#include "NumberField.h"
//
////NumberField::NumberField() : Gtk::Entry()
////{
////	set_input_purpose(Gtk::InputPurpose::DIGITS);
////	set_input_hints(Gtk::InputHints::NONE);
////	
////	//signal_insert_text().connect(sigc::mem_fun(*this, &NumberField::OnInsertText));
////	//signal_changed().connect(sigc::mem_fun(*this, &NumberField::OnChanged), false);
////}
////NumberField::~NumberField()
////{
////
////}
////
////void NumberField::OnInsertText(const Glib::ustring& text, int* position)
////{
////	Glib::ustring currentText = get_text();
////	Glib::ustring filteredText = "";
////
////	for(char c : text)
////	{
////		if(std::isdigit(c))
////			filteredText += c;
////		else if(c == '-')
////			if(*position == 0 && currentText.find('-') == Glib::ustring::npos)
////				filteredText += c;
////		else if(c == '.')
////			if(currentText.find('.') == Glib::ustring::npos)
////				filteredText += c;
////	}
////
////	if(!filteredText.empty())
////		Gtk::Entry::on_insert_text(filteredText, position);
////}
////void NumberField::OnChanged()
////{
////	Glib::ustring currentText = get_text();
////	Glib::ustring filteredText = "";
////	int position = get_position();
////
////	for(char c : currentText)
////	{
////		if(std::isdigit(c))
////		{
////			filteredText += c;
////			//position--;
////		}
////		else if(c == '-')
////			if(position == 0 && currentText.find('-') == Glib::ustring::npos)
////			{
////				filteredText += c;
////				//position--;
////			}
////		else if(c == '.')
////			if(currentText.find('.') == Glib::ustring::npos)
////			{
////				filteredText += c;
////				//position--;
////			}
////	}
////
////	if(!(filteredText == currentText))
////	{
////		// Block the signal to prevent recursion when setting the text
////		//m_connection_changed.block();
////
////		set_text(filteredText);
////		set_position(position);
////
////		//m_connection_changed.unblock();
////	}
////}