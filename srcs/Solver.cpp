#include "../includes/Solver.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>

static bool	lessReal(const Complex &a, const Complex &b)	{	return a.re().value() < b.re().value();	}

Solution	Solver::solve(const Polynomial &p)
{
	Solution	s;
	
	if (p.isZero())
	{
		s.kind = Solution::ALL;
		return s;
	}
	if (p.degree() == 0)
		return s;
	if (p.degree() > 2)
	{
		std::ostringstream	msg;
		msg << "the polynomial has degree " << p.degree() << ": only degree 2 or less can be solved";
		throw std::runtime_error(msg.str());
	}

	s.kind = Solution::ROOTS;
	if (p.degree() == 1)
		s.roots.push_back(-p.coef(0) / p.coef(1));
	else
	{
		Complex	a = p.coef(2), b = p.coef(1), c = p.coef(0);
		Complex	disc = b * b - Complex(4) * a * c;
		Complex	twoA = Complex(2) * a;
		if (disc.isZero())
			s.roots.push_back(-b / twoA);
		else
		{
			Complex	root = disc.sqrt();
			s.roots.push_back((-b - root) / twoA);
			s.roots.push_back((-b + root) / twoA);
		}
	}
	for (size_t k = 0; k < s.roots.size(); k++)
		if (!s.roots[k].isReal())
			s.real = false;
	if (s.real)
		std::sort(s.roots.begin(), s.roots.end(), lessReal);
	return s;
}
