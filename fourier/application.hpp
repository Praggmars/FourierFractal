#pragma once

#include "plot.hpp"
#include <memory>

namespace fourier
{
	class Application
	{
		struct PathGenerationInfo
		{
			size_t fractalIters;
			size_t newtonApproxIters;
			size_t pointCount;
			double escapeRadius;
		};

	private:
		GtkApplication* m_gtkApp;
		std::unique_ptr<Plot> m_plot;
		std::vector<double2> m_path;
		PathGenerationInfo m_pathInfo;

	private:
		void Activate();

		void GeneratePath();

		void SaveToWav() const;

	public:
		Application();
		~Application();

		int Main(int argc, char* argv[]);
	};
}
