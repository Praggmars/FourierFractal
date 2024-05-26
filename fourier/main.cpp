#include "application.hpp"
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>

using namespace fourier;

class AppTimer
{
	std::chrono::steady_clock::time_point m_startTime;

public:
	AppTimer()
		: m_startTime{std::chrono::steady_clock::now()}
	{}
	~AppTimer()
	{
		std::cout << "Application run time: " <<
				std::chrono::duration<double>(std::chrono::steady_clock::now() - m_startTime).count() <<
				" seconds. " << std::endl;
	}
};

int main (int argc, char **argv)
{
#if 0
	struct Sample
	{
		int16_t first;
		int16_t second;
	};

#if 0
	std::ifstream infile("Asine.wav", std::ios::binary);
	if (infile.is_open())
	{
		WavHeader header;
		infile.read((char*)&header, sizeof(header));
		std::vector<Sample> data(header.dataSectionSize / 4);
		infile.read((char*)data.data(), header.dataSectionSize);
		for (Sample s : data)
			std::cout << s.first << ' ' << s.second << '\n';
	}
#endif

	std::ofstream of("audio.wav", std::ios::binary);
	if (of.is_open())
	{
		std::vector<Sample> data;
		data.resize(44100 * 5);
		for (size_t i = 0; i < data.size(); ++i)
		{
			int16_t p = static_cast<int16_t>(30256.0 * std::sin(static_cast<double>(i) * 2.0 * G_PI / 44100.0 * 400.0));
			data[i].first = data[i].second = p;
		}

		WavHeader header;
		header.riff[0] = 'R';
		header.riff[1] = 'I';
		header.riff[2] = 'F';
		header.riff[3] = 'F';
		header.fileSize = sizeof(header) - 8 + sizeof(Sample) * data.size();
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
		header.dataSectionSize = sizeof(Sample) * data.size();
		of.write((const char*)&header, sizeof(header));
		of.write((const char*)data.data(), header.dataSectionSize);
	}
	return 0;
#endif

	AppTimer timer;
	fourier::Application application;
	return application.Main(argc, argv);
}
