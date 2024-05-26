#pragma once

#include "panandzoom.hpp"

namespace fourier
{
	class Plot : public PanAndZoom
	{
		const std::vector<double2>* m_points;

	protected:
		virtual void Draw(cairo_t* cr, int w, int h) const override;

	public:
		Plot();

		void SetPoints(const std::vector<double2>* points);
	};
}
