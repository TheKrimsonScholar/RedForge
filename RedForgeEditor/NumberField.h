#pragma once

#include "gtkmm/entry.h"
#include <gtkmm/gestureclick.h>
#include <gtkmm/eventcontrollerfocus.h>
#include <gtkmm/eventcontrollermotion.h>
#include <iomanip>

#include <iostream>

class DragFloat : public Gtk::Entry
{
private:
    double value = 0.0;
    double min_value = -std::numeric_limits<double>::max();
    double max_value = std::numeric_limits<double>::max();
    double speed = 1.0;
    int decimal_places = 3;

    bool is_dragging = false;
    double drag_start_x = 0.0;
    double drag_start_value = 0.0;
    bool is_filtering = false;

    Glib::RefPtr<Gtk::EventControllerMotion> motion_controller;
    Glib::RefPtr<Gtk::GestureClick> click_gesture;

public:
    DragFloat(double initial_value = 0.0, double drag_speed = 1.0,
        double min_val = -std::numeric_limits<double>::max(),
        double max_val = std::numeric_limits<double>::max(),
        int decimals = 3)
        : value(initial_value), speed(drag_speed), min_value(min_val),
        max_value(max_val), decimal_places(decimals)
    {
        add_css_class("drag-scalar");

        // Set initial text
        update_display();

        // Set up text filtering
        signal_changed().connect(sigc::mem_fun(*this, &DragFloat::on_text_changed));

        // Set up focus controller for focus out events
        auto focus_controller = Gtk::EventControllerFocus::create();
        focus_controller->signal_leave().connect(sigc::mem_fun(*this, &DragFloat::on_focus_out));
        add_controller(focus_controller);

        // Set up mouse event handling for dragging
        // Use click gesture to detect press/release
        click_gesture = Gtk::GestureClick::create();
        click_gesture->set_button(GDK_BUTTON_PRIMARY);
        click_gesture->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
        click_gesture->signal_pressed().connect(
            sigc::mem_fun(*this, &DragFloat::on_mouse_press));
        click_gesture->signal_released().connect(
            sigc::mem_fun(*this, &DragFloat::on_mouse_release));
        add_controller(click_gesture);

        // Use motion controller for tracking movement
        motion_controller = Gtk::EventControllerMotion::create();
        motion_controller->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
        motion_controller->signal_motion().connect(
            sigc::mem_fun(*this, &DragFloat::on_mouse_motion));
        add_controller(motion_controller);

        // Change cursor when hovering
        set_cursor(Gdk::Cursor::create("ew-resize"));
    }

    // Getters and setters
    double get_value() const { return value; }
    void set_value(double val)
    {
        value = std::clamp(val, min_value, max_value);
        update_display();
    }

    void set_range(double min_val, double max_val)
    {
        min_value = min_val;
        max_value = max_val;
        set_value(value); // Clamp current value
    }

    void set_speed(double drag_speed) { speed = drag_speed; }
    void set_decimal_places(int decimals)
    {
        decimal_places = decimals;
        update_display();
    }

    // Signal for value changes
    sigc::signal<void(double)> signal_value_changed() { return m_signal_value_changed; }

private:
    sigc::signal<void(double)> m_signal_value_changed;

    void update_display()
    {
        if(is_filtering)
            return;

        is_filtering = true;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(decimal_places) << value;
        set_text(oss.str());
        is_filtering = false;
    }

    void on_text_changed()
    {
        if (is_filtering || is_dragging) return;

        std::string text = get_text();
        try {
            double new_value = std::stod(text);
            if (new_value != value) {
                value = std::clamp(new_value, min_value, max_value);
                m_signal_value_changed.emit(value);
            }
        }
        catch (const std::exception&) {
            // Invalid input, revert to last valid value
            update_display();
        }
    }

    void on_focus_out() {
        // Ensure display is up to date when losing focus
        update_display();
    }

    void on_mouse_press(int n_press, double x, double y)
    {
        std::cout << "Mouse press at: " << x << ", " << y << std::endl;
        is_dragging = true;
        drag_start_x = x;
        drag_start_value = value;

        // Prevent text selection during drag
        set_editable(false);

        // Grab focus but don't place cursor
        grab_focus();
    }

    void on_mouse_motion(double x, double y)
    {
        if (!is_dragging) return;

        double offset_x = x - drag_start_x;
        std::cout << "Mouse motion - offset: " << offset_x << std::endl;

        // Calculate new value based on horizontal mouse movement
        double delta = offset_x * speed * 0.1;
        double new_value = drag_start_value + delta;

        // Apply constraints
        new_value = std::clamp(new_value, min_value, max_value);

        std::cout << "Old value: " << value << ", New value: " << new_value << std::endl;

        if (std::abs(new_value - value) > 0.001) {
            value = new_value;
            update_display();
            m_signal_value_changed.emit(value);
        }
    }

    void on_mouse_release(int n_press, double x, double y)
    {
        std::cout << "Mouse release" << std::endl;
        if (is_dragging) {
            is_dragging = false;

            // Re-enable text editing
            set_editable(true);

            // If there was minimal movement, allow normal text editing
            double offset_x = x - drag_start_x;
            if (std::abs(offset_x) < 5.0) { // Less than 5 pixels = click, not drag
                // Position cursor at click location
                set_position(-1); // Move to end, or you could calculate position from x coordinate
            }
        }
    }
};

//class NumberField : public Gtk::Entry
//{
//public:
//	NumberField();
//	~NumberField();
//
//protected:
//	void OnInsertText(const Glib::ustring& text, int* position);
//	void OnChanged();
//};