#pragma once

#include "InputLayer.h"

#include "gtkmm.h"

class GTKInputLayer : public InputLayer
{
private:
	Gtk::Widget* widget;

	Glib::RefPtr<Gtk::EventControllerMotion> m_mouseController;
	Glib::RefPtr<Gtk::GestureClick> m_clickGesture;
	Glib::RefPtr<Gtk::EventControllerKey> m_keyController;

public:
	GTKInputLayer(Gtk::Widget* widget);
	~GTKInputLayer() override;

	void PreUpdate() override;
	void PostUpdate() override;

	void OnMouseMotion(double x, double y);

	void OnMouseButtonPressed(int n_press, double x, double y);
	void OnMouseButtonReleased(int n_press, double x, double y);

	bool OnKeyPressed(guint keyval, guint keycode, Gdk::ModifierType state);
	void OnKeyReleased(guint keyval, guint keycode, Gdk::ModifierType state);
};