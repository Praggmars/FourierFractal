#include "types.hpp"

namespace fourier
{
	Polinom::Polinom(){}

	Polinom::Polinom(const std::initializer_list<double>& polinom)
	{
		m_polinom.reserve(polinom.size());
		for (double p : polinom)
			m_polinom.emplace_back(p);
	}

	Polinom Polinom::Derivative() const
	{
		Polinom r;
		if (m_polinom.size() > 1)
		{
			size_t power = m_polinom.size() - 1;
			r.m_polinom.resize(power);
			for (size_t i = 0; i < r.m_polinom.size(); ++i)
				r.m_polinom[i] = m_polinom[i] * static_cast<double>(power--);
		}
		return r;
	}

	Polinom Polinom::operator*(const Polinom& rhs) const
	{
		Polinom r;

		if (!m_polinom.empty() && !rhs.m_polinom.empty())
		{
			r.m_polinom.resize(m_polinom.size() + rhs.m_polinom.size() - 1);
			for (double& d : r.m_polinom)
				d = 0.0;
			for (size_t lhsInd = 0; lhsInd < m_polinom.size(); ++lhsInd)
				for (size_t rhsInd = 0; rhsInd < rhs.m_polinom.size(); ++rhsInd)
					r.m_polinom[lhsInd + rhsInd] += m_polinom[lhsInd] * rhs.m_polinom[rhsInd];
		}

		return r;
	}

	Polinom Polinom::operator+(const Polinom& rhs) const
	{
		Polinom r;
		const std::vector<double> *smaller, *bigger;
		if (m_polinom.size() > rhs.m_polinom.size())
		{
			smaller = &rhs.m_polinom;
			bigger = &m_polinom;
		}
		else
		{
			bigger = &rhs.m_polinom;
			smaller = &m_polinom;
		}
		r.m_polinom = *bigger;
		const size_t sizeDiff = bigger->size() - smaller->size();
		for (size_t i = 0; i < smaller->size(); ++i)
			r.m_polinom[sizeDiff + i] += (*smaller)[i];

		return r;
	}

	void Polinom::Print(std::ostream& os) const
	{
		size_t power;
		size_t endIndMinus2;

		switch (m_polinom.size())
		{
		case 0:
			break;
		case 1:
			os << m_polinom[0];
			break;
		default:
			power = m_polinom.size() - 1;
			endIndMinus2 = power - 1;
			for (size_t i = 0; i < endIndMinus2; ++i)
				os << m_polinom[i] << "*z^" << power-- << '+';
			os << m_polinom[endIndMinus2] << "*z+";
			os << m_polinom[endIndMinus2 + 1];
		}
	}

	std::ostream& operator<<(std::ostream& os, const Polinom& polinom)
	{
		polinom.Print(os);
		return os;
	}
}
