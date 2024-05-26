#include "application.hpp"
#include "common.hpp"
#include <fstream>

namespace fourier
{
	void Application::Activate()
	{
		m_pathInfo.fractalIters = 5;
		m_pathInfo.newtonApproxIters = 10;
		m_pathInfo.pointCount = 1000;
		m_pathInfo.escapeRadius = 2.0;

		GtkWidget* window = gtk_application_window_new(m_gtkApp);
		gtk_window_set_title(GTK_WINDOW(window), "Fourier");
		gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);

		GtkWidget* container = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
		gtk_window_set_child(GTK_WINDOW(window), container);

		GtkWidget* menu = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
		gtk_widget_set_size_request(menu, 200, 300);
		gtk_paned_set_start_child(GTK_PANED(container), menu);
		gtk_paned_set_shrink_start_child(GTK_PANED(container), false);

		{
			GtkWidget* fractalItersSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 10.0, 1.0);
			gtk_range_set_value(GTK_RANGE(fractalItersSlider), static_cast<double>(m_pathInfo.fractalIters));
			g_signal_connect(fractalItersSlider, "value-changed", G_CALLBACK(static_cast<void(*)(GtkRange*, Application*)>([](GtkRange* range, Application* app){
				app->m_pathInfo.fractalIters = static_cast<size_t>(gtk_range_get_value(range));
				app->GeneratePath();
			})), this);
			gtk_scale_set_draw_value(GTK_SCALE(fractalItersSlider), true);
			GtkWidget* fractalItersFrame = gtk_frame_new("Fractal iteration count");
			gtk_frame_set_child(GTK_FRAME(fractalItersFrame), fractalItersSlider);
			gtk_box_append(GTK_BOX(menu), fractalItersFrame);
		}
		{
			GtkWidget* newtonItersSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0);
			gtk_range_set_value(GTK_RANGE(newtonItersSlider), static_cast<double>(m_pathInfo.newtonApproxIters));
			g_signal_connect(newtonItersSlider, "value-changed", G_CALLBACK(static_cast<void(*)(GtkRange*, Application*)>([](GtkRange* range, Application* app){
				app->m_pathInfo.newtonApproxIters = static_cast<size_t>(gtk_range_get_value(range));
				app->GeneratePath();
			})), this);
			gtk_scale_set_draw_value(GTK_SCALE(newtonItersSlider), true);
			GtkWidget* newtonItersFrame = gtk_frame_new("Newton approximation iteration count");
			gtk_frame_set_child(GTK_FRAME(newtonItersFrame), newtonItersSlider);
			gtk_box_append(GTK_BOX(menu), newtonItersFrame);
		}
		{
			GtkWidget* pointCountSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 10000.0, 1.0);
			gtk_range_set_value(GTK_RANGE(pointCountSlider), static_cast<double>(m_pathInfo.pointCount));
			g_signal_connect(pointCountSlider, "value-changed", G_CALLBACK(static_cast<void(*)(GtkRange*, Application*)>([](GtkRange* range, Application* app){
				app->m_pathInfo.pointCount = static_cast<size_t>(gtk_range_get_value(range));
				app->GeneratePath();
			})), this);
			gtk_scale_set_draw_value(GTK_SCALE(pointCountSlider), true);
			GtkWidget* pointCountFrame = gtk_frame_new("Outline point count");
			gtk_frame_set_child(GTK_FRAME(pointCountFrame), pointCountSlider);
			gtk_box_append(GTK_BOX(menu), pointCountFrame);
		}
		{
			GtkWidget* escapeRadiusSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1.0, 2.0, 0.001);
			gtk_range_set_value(GTK_RANGE(escapeRadiusSlider), static_cast<double>(m_pathInfo.escapeRadius));
			g_signal_connect(escapeRadiusSlider, "value-changed", G_CALLBACK(static_cast<void(*)(GtkRange*, Application*)>([](GtkRange* range, Application* app){
				app->m_pathInfo.escapeRadius = static_cast<size_t>(gtk_range_get_value(range));
				app->GeneratePath();
			})), this);
			gtk_scale_set_draw_value(GTK_SCALE(escapeRadiusSlider), true);
			GtkWidget* escapeRadiusFrame = gtk_frame_new("Escape radius");
			gtk_frame_set_child(GTK_FRAME(escapeRadiusFrame), escapeRadiusSlider);
			gtk_box_append(GTK_BOX(menu), escapeRadiusFrame);
		}
		GtkWidget* saveButton = gtk_button_new();
		gtk_button_set_label(GTK_BUTTON(saveButton), "Save sound");
		g_signal_connect(saveButton, "clicked", G_CALLBACK(static_cast<void(*)(GtkButton*, Application*)>([](GtkButton*, Application* app){
			app->SaveToWav();
		})), this);
		gtk_box_append(GTK_BOX(menu), saveButton);

		m_plot = std::make_unique<Plot>();
		gtk_paned_set_end_child(GTK_PANED(container), GTK_WIDGET(m_plot->Canvas()));
		gtk_paned_set_shrink_end_child(GTK_PANED(container), false);

		gtk_paned_set_position(GTK_PANED(container), 300);

		GeneratePath();

		gtk_window_present(GTK_WINDOW(window));
	}

	void Application::GeneratePath()
	{
		m_path.clear();

		const Polinom eq({1.0, 0.0});
		Polinom polinom = eq;
		for (size_t i = 0; i < m_pathInfo.fractalIters; ++i)
			polinom = polinom * polinom + eq;

		std::complex<double> z(0.25, 0.0);
		for (size_t pointIndex = 0; pointIndex < m_pathInfo.pointCount; ++pointIndex)
		{
			z = polinom.FindZero(z,
					m_pathInfo.escapeRadius * std::exp(
							std::complex<double>(0.0, static_cast<double>(pointIndex * (1 << m_pathInfo.fractalIters)) / static_cast<double>(m_pathInfo.pointCount - 1) * G_PI * 2.0)),
					m_pathInfo.newtonApproxIters);
			m_path.emplace_back(z);
		}

		m_plot->SetPoints(&m_path);
	}

	void Application::SaveToWav() const
	{
		struct Sample
		{
			int16_t first;
			int16_t second;
		};
		std::ofstream of("audio.wav", std::ios::binary);
		constexpr size_t repeatCount = 100;

		WavHeader header;
		header.riff[0] = 'R';
		header.riff[1] = 'I';
		header.riff[2] = 'F';
		header.riff[3] = 'F';
		header.fileSize = sizeof(header) - 8 + sizeof(Sample) * m_path.size() * repeatCount;
		header.wave[0] = 'W';
		header.wave[1] = 'A';
		header.wave[2] = 'V';
		header.wave[3] = 'E';
		header.fmt_[0] = 'f';
		header.fmt_[1] = 'm';
		header.fmt_[2] = 't';
		header.fmt_[3] = ' ';
		header.wavSectionSize = 16;
		header.wavTypeFormat = 1;
		header.monoStereo = 2;
		header.sampleFreq = 44100;
		header.dataRate = 44100 * 4;
		header.blockAlignment = 4;
		header.bitsPerSample = 16;
		header.data[0] = 'd';
		header.data[1] = 'a';
		header.data[2] = 't';
		header.data[3] = 'a';
		header.dataSectionSize = sizeof(Sample) * m_path.size() * repeatCount;
		of.write((const char*)&header, sizeof(header));

		for (size_t i = 0; i < repeatCount; ++i)
		{
			for (double2 p : m_path)
			{
				const Sample s{ static_cast<int16_t>(p.x * 16000.0), static_cast<int16_t>(p.y * 16000.0) };
				of.write((const char*)&s, sizeof(s));
			}
		}
	}

	Application::Application()
		: m_gtkApp{nullptr}
	{}

	Application::~Application()
	{}

	int Application::Main(int argc, char* argv[])
	{
		m_gtkApp = gtk_application_new("app.filtering.filtering", G_APPLICATION_DEFAULT_FLAGS);
		g_signal_connect(m_gtkApp, "activate", G_CALLBACK(static_cast<void(*)(GtkApplication*, Application*)>(
				[](GtkApplication*, Application* app)->void{
			app->Activate();
		})), this);
		int status = g_application_run(G_APPLICATION(m_gtkApp), argc, argv);
		g_object_unref(m_gtkApp);
		return status;
	}
}
