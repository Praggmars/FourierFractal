#include "panandzoom.hpp"

namespace fourier
{
	void PanAndZoom::MouseMove(double2 cursor)
	{
		if (m_tracking)
		{
			m_screenTransform.Track(cursor - m_cursor);
			RequestRedraw();
		}
		m_cursor = cursor;
	}

	void PanAndZoom::Scroll(double delta)
	{
		m_screenTransform.Zoom(m_cursor, delta > 0 ? 1.1 : 1.0 / 1.1);
		RequestRedraw();
	}

	void PanAndZoom::RequestRedraw()
	{
		gtk_widget_queue_draw(GTK_WIDGET(m_canvas));
	}

	PanAndZoom::PanAndZoom()
		: m_tracking{false}
		, m_screenTransform(double2(), Rectangled{double2(-1.0), double2(2.0)})
	{
		m_canvas = GTK_DRAWING_AREA(gtk_drawing_area_new());
		gtk_drawing_area_set_draw_func(m_canvas, static_cast<GtkDrawingAreaDrawFunc>(
				[](GtkDrawingArea*, cairo_t* cr, int w, int h, gpointer data)->void{
			reinterpret_cast<PanAndZoom*>(data)->Draw(cr, w, h);
		}), static_cast<gpointer>(this), nullptr);
		g_signal_connect_after(m_canvas, "resize", G_CALLBACK(static_cast<void(*)(GtkDrawingArea*, gint, gint, PanAndZoom*)>(
				[](GtkDrawingArea*, gint width, gint height, PanAndZoom* paz)->void{
			paz->m_screenTransform.ChangeScreenSize(double2(width, height));
		})), this);

		m_mouseMotion = GTK_EVENT_CONTROLLER_MOTION(gtk_event_controller_motion_new());
		gtk_widget_add_controller(GTK_WIDGET(m_canvas), GTK_EVENT_CONTROLLER(m_mouseMotion));
		g_signal_connect(m_mouseMotion, "motion", G_CALLBACK(static_cast<void(*)(GtkEventControllerMotion*, double, double, PanAndZoom*)>(
				[](GtkEventControllerMotion*, double x, double y, PanAndZoom* paz)->void{
			paz->MouseMove(double2(x, y));
		})), this);

		m_drag = GTK_GESTURE_DRAG(gtk_gesture_drag_new());
		gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(m_drag), GDK_BUTTON_PRIMARY);
		gtk_widget_add_controller(GTK_WIDGET(m_canvas), GTK_EVENT_CONTROLLER(m_drag));
		g_signal_connect(m_drag, "drag-begin", G_CALLBACK(static_cast<void(*)(GtkGestureDrag*, double, double, PanAndZoom*)>(
				[](GtkGestureDrag*, double x, double y, PanAndZoom* paz)->void{
			paz->m_tracking = true;
		})), this);
		g_signal_connect(m_drag, "drag-end", G_CALLBACK(static_cast<void(*)(GtkGestureDrag*, double, double, PanAndZoom*)>(
				[](GtkGestureDrag*, double x, double y, PanAndZoom* paz)->void{
			paz->m_tracking = false;
		})), this);

		m_scroll = GTK_EVENT_CONTROLLER_SCROLL(gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL));
		gtk_widget_add_controller(GTK_WIDGET(m_canvas), GTK_EVENT_CONTROLLER(m_scroll));
		g_signal_connect(m_scroll, "scroll", G_CALLBACK(static_cast<void(*)(GtkEventControllerScroll*, double, double, PanAndZoom*)>(
				[](GtkEventControllerScroll*, double x, double y, PanAndZoom* paz)->void{
			paz->Scroll(y);
		})), this);
	}

	PanAndZoom::~PanAndZoom(){}
}
