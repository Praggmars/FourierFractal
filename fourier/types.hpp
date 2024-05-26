#pragma once

#include <cstdint>
#include <complex>
#include <vector>

namespace fourier
{
	struct WavHeader
	{
		char riff[4];
		uint32_t fileSize;
		char wave[4];
		char fmt_[4];
		uint32_t wavSectionSize;
		uint16_t wavTypeFormat;	// 1
		uint16_t monoStereo;	// 1/2
		uint32_t sampleFreq;
		uint32_t dataRate;		// bytes/sec
		uint16_t blockAlignment;
		uint16_t bitsPerSample;
		char data[4];
		uint32_t dataSectionSize;
	};

	template <typename T>
	class vec2
	{
	public:
		T x;
		T y;

	public:
		vec2() : x{T{0}}, y{T{0}} {}
		vec2(const T& v) : x{v}, y{v} {}
		vec2(const T& x, const T& y) : x{x}, y{y} {}
		vec2(const std::complex<T>& z) : x{z.real()}, y{z.imag()} {}

		vec2<T> operator+(const vec2<T>& rhs) const { return vec2<T>(x + rhs.x, y + rhs.y); }
		vec2<T> operator-(const vec2<T>& rhs) const { return vec2<T>(x - rhs.x, y - rhs.y); }
		vec2<T> operator*(const vec2<T>& rhs) const { return vec2<T>(x * rhs.x, y * rhs.y); }
		vec2<T> operator/(const vec2<T>& rhs) const { return vec2<T>(x / rhs.x, y / rhs.y); }

		vec2<T> operator+(const T& rhs) const { return vec2<T>(x + rhs, y + rhs); }
		vec2<T> operator-(const T& rhs) const { return vec2<T>(x - rhs, y - rhs); }
		vec2<T> operator*(const T& rhs) const { return vec2<T>(x * rhs, y * rhs); }
		vec2<T> operator/(const T& rhs) const { return vec2<T>(x / rhs, y / rhs); }

		const vec2<T>& operator+=(const vec2<T>& rhs) { x += rhs.x; y += rhs.y; return *this; }
		const vec2<T>& operator-=(const vec2<T>& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		const vec2<T>& operator*=(const vec2<T>& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
		const vec2<T>& operator/=(const vec2<T>& rhs) { x /= rhs.x; y /= rhs.y; return *this; }

		const vec2<T>& operator+=(const T& rhs) { x += rhs; y += rhs; return *this; }
		const vec2<T>& operator-=(const T& rhs) { x -= rhs; y -= rhs; return *this; }
		const vec2<T>& operator*=(const T& rhs) { x *= rhs; y *= rhs; return *this; }
		const vec2<T>& operator/=(const T& rhs) { x /= rhs; y /= rhs; return *this; }

		explicit operator std::complex<T>() const { return std::complex<T>(x, y); }
	};

	class Polinom
	{
		std::vector<double> m_polinom;

	public:
		Polinom();
		Polinom(const std::initializer_list<double>& polinom);
		Polinom Derivative() const;
		Polinom operator*(const Polinom& rhs) const;
		Polinom operator+(const Polinom& rhs) const;

		template <typename T>
		T Evaluate(const T& z) const
		{
#if 0
			T zz{};
			T comp{};
			const size_t polSize = m_polinom.size();
			for (size_t i = 0; i < polSize; ++i)
			{
				T addand = m_polinom[i] * std::pow(z, polSize - i);
				T y = addand - comp;
				T t = zz + y;
				comp = (t - zz)  - y;
				zz = t;
			}
			return zz;
#else
			if (m_polinom.empty())
				return T{};
			size_t i = m_polinom.size() - 1;
			T zz = z;
			T r = m_polinom[i];
			T comp{};
			while (i > 0)
			{
				T addand = m_polinom[--i] * zz;
				T y = addand - comp;
				T t = r + y;
				comp = (t - r) - y;
				r = t;
				zz *= z;
			}
			return r;
#endif
		}

		template <typename T>
		T FindZero(T z, const T& rhs, size_t iterations) const
		{
			Polinom der = Derivative();
			for (size_t i = 0; i < iterations; ++i)
				z -= 0.5 * (Evaluate(z) - rhs) / der.Evaluate(z);
			return z;
		}

		void Print(std::ostream& os) const;
	};

	std::ostream& operator<<(std::ostream& os, const Polinom& polinom);

	template <typename T>
	class Rectangle
	{
	public:
		vec2<T> position;
		vec2<T> size;
	};

	template <typename T>
	class ScreenTransform
	{
		vec2<T> m_screenSize;
		Rectangle<T> m_visibleArea;

	public:
		ScreenTransform()
			: m_screenSize{}
			, m_visibleArea{}
		{}

		ScreenTransform(const vec2<T>& screenSize, const Rectangle<T>& visibleArea)
			: m_screenSize{screenSize}
			, m_visibleArea{visibleArea}
		{}

		void ChangeScreenSize(const vec2<T>& screenSize)
		{
			m_screenSize = screenSize;
		}

		void Zoom(const vec2<T>& around, const T& zoom)
		{
			const vec2<T> center = ToVisibleCoords(around);
			m_visibleArea.position = (m_visibleArea.position - center) * zoom + center;
			m_visibleArea.size *= zoom;
		}

		void Track(const vec2<T>& delta)
		{
			m_visibleArea.position += ToVisibleDelta(delta);
		}

		vec2<T> ToScreenCoords(const vec2<T>& point) const
		{
			return vec2<T>(
					(point.x - m_visibleArea.position.x) / m_visibleArea.size.x * (m_screenSize.x - T{1}),
					(m_screenSize.y - T{1}) - (point.y - m_visibleArea.position.y) / m_visibleArea.size.y * (m_screenSize.y - T{1})
			);
		}

		vec2<T> ToScreenDelta(const vec2<T>& delta) const
		{
			return vec2<T>(
					delta.x / m_visibleArea.size.x * (m_screenSize.x - T{1}),
					delta.y / m_visibleArea.size.y * (T{1} - m_screenSize.y)
			);
		}

		vec2<T> ToVisibleCoords(const vec2<T>& point) const
		{
			return vec2<T>(
					point.x * m_visibleArea.size.x / (m_screenSize.x - T{1}) + m_visibleArea.position.x,
					((m_screenSize.y - T{1}) - point.y) * m_visibleArea.size.y / (m_screenSize.y - T{1}) + m_visibleArea.position.y
			);
		}

		vec2<T> ToVisibleDelta(const vec2<T>& delta) const
		{
			return vec2<T>(
					delta.x * m_visibleArea.size.x / (T{1} - m_screenSize.x),
					delta.y * m_visibleArea.size.y / (m_screenSize.y - T{1})
			);
		}

		inline const vec2<T>& ScreenSize() const { return m_screenSize; }
		inline const Rectangle<T>& VisibleArea() const { return m_visibleArea; }
	};

	using double2 = vec2<double>;
	using Rectangled = Rectangle<double>;
	using ScreenTransformd = ScreenTransform<double>;
}
