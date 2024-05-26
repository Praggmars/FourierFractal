#include "plot.hpp"

namespace fourier
{
	void Plot::Draw(cairo_t* cr, int w, int h) const
	{
		if (m_points)
		{
			double2 p;
			switch (m_points->size())
			{
			case 0:
				break;
			case 1:
				p = m_screenTransform.ToScreenCoords((*m_points)[0]);
				cairo_arc(cr, p.x, p.y, 3.0, 0, 2 * G_PI);
				cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
				cairo_fill(cr);
				break;
			default:
				p = m_screenTransform.ToScreenCoords((*m_points)[0]);
				cairo_move_to(cr, p.x, p.y);
				for (size_t i = 1; i < m_points->size(); ++i)
				{
					p = m_screenTransform.ToScreenCoords((*m_points)[i]);
					cairo_line_to(cr, p.x, p.y);
				}
				cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
				cairo_set_line_width(cr, 3.0);
				cairo_stroke(cr);
				break;
			}
		}
	}

	Plot::Plot()
		: PanAndZoom()
		, m_points{nullptr}
	{}

	void Plot::SetPoints(const std::vector<double2>* points)
	{
		m_points = points;
		RequestRedraw();
	}
}
