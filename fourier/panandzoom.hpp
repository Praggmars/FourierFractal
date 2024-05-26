#pragma once

#include <gtk/gtk.h>
#include <vector>
#include "types.hpp"

namespace fourier
{
	class PanAndZoom
	{
	protected:
		GtkDrawingArea* m_canvas;
		double2 m_cursor;
		bool m_tracking;
		GtkEventControllerMotion* m_mouseMotion;
		GtkGestureDrag* m_drag;
		GtkEventControllerScroll* m_scroll;
		ScreenTransformd m_screenTransform;

	protected:
		virtual void MouseMove(double2 cursor);
		virtual void Scroll(double delta);
		void RequestRedraw();
		virtual void Draw(cairo_t* cr, int w, int h) const = 0;

	public:
		PanAndZoom();
		virtual ~PanAndZoom();

		inline GtkDrawingArea* Canvas() const { return m_canvas; }
	};
}
