#pragma once

#include <vector>
#include <functional>
#include <cmath>
#include <complex>

namespace fourier
{
	template <typename T, size_t CAPACITY>
	class Fifo
	{
		T m_fifo[CAPACITY];
		size_t m_first;
		size_t m_size;

	public:
		Fifo()
			: m_fifo{}
			, m_first{}
			, m_size{}
		{}

		inline size_t Size() const
		{
			return m_size;
		}

		bool Push(const T& element)
		{
			if (m_size < CAPACITY)
			{
				m_fifo[(m_first + m_size) % CAPACITY] = element;
				++m_size;
				return true;
			}
			return false;
		}

		T&& Pop()
		{
			if (m_size)
			{
				const size_t index = m_first;
				m_first = (m_first + 1) % CAPACITY;
				--m_size;
				return m_fifo[index];
			}
			return T{};
		}

		void PopDiscard(size_t count = 1)
		{
			if (m_size < count)
				count = m_size;
			m_first = (m_first + count) % CAPACITY;
			m_size -= count;
		}

		const T& operator[](size_t index) const
		{
			if (index < m_size)
				return m_fifo[(m_first + index) % CAPACITY];
			return T{};
		}
	};

	template <typename T>
	std::vector<vec2<T>> PlotFunction(const T& from, const T& to, const T& delta, std::function<T(T)> func)
	{
		std::vector<vec2<T>> points;
		for (T x = from; x <= to; x += delta)
			points.emplace_back(x, func(x));
		return points;
	}

	template <typename T>
	std::vector<vec2<T>> FourierPoints(const std::complex<T> coefficients[], size_t coeffCount, size_t pointCount)
	{
		std::vector<vec2<T>> points;
		for (size_t pointIndex = 0; pointIndex < pointCount; ++pointIndex)
		{
			const double r = static_cast<T>(pointIndex) / static_cast<T>(pointCount - 1) * G_PI * 2.0;
			std::complex<T> p;
			for (size_t coeffIndex = 0; coeffIndex < coeffCount; ++coeffIndex)
				p += coefficients[coeffIndex] * std::exp(std::complex<T>(T{0}, static_cast<T>(coeffIndex) * r));
			points.emplace_back(p);
		}

		return points;
	}

	template <typename T>
	T sinc(T x)
	{
		return x ? std::sin(x) / x : T{1};
	}
}
