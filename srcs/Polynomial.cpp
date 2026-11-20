#include "../includes/Polynomial.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>

Polynomial::Polynomial() {}

Polynomial::Polynomial(const Complex &constant)
{
	_c.push_back(constant);
	trim();
}

Polynomial	Polynomial::x()
{
	Polynomial	p;
	p._c.push_back(Complex(0));
	p._c.push_back(Complex(1));
	return p;
}

void	Polynomial::trim()
{
	while (!_c.empty() && _c.back().isZero())
		_c.pop_back();
	if (static_cast<int>(_c.size()) - 1 > MAX_DEGREE)
		throw std::runtime_error("polynomial degree too large");
}

int	Polynomial::degree() const	{	return static_cast<int>(_c.size()) - 1;	}

bool	Polynomial::isZero() const	{	return _c.empty();	}

bool	Polynomial::isConstant() const	{	return _c.size() <= 1;	}

bool	Polynomial::hasRealCoefficients() const
{
	for (size_t k = 0; k < _c.size(); k++)
		if (!_c[k].isReal())
			return false;
	return true;
}

const Complex	&Polynomial::coef(int k) const
{
	static const	Complex	zero(0);
	if (k < 0 || k >= static_cast<int>(_c.size()))
		return zero;
	return _c[k];
}

Complex	Polynomial::evaluate(const Complex &x) const
{
	Complex	r(0);
	for (int k = degree(); k >= 0; k--)
		r = r * x + _c[k];
	return r;
}

Polynomial	Polynomial::operator+(const Polynomial &o) const
{
	Polynomial	p;
	size_t	n = std::max(_c.size(), o._c.size());
	for (size_t k = 0; k < n; k++)
		p._c.push_back(coef(static_cast<int>(k)) + o.coef(static_cast<int>(k)));
	p.trim();
	return p;
}

Polynomial	Polynomial::operator-(const Polynomial &o) const
{
	return (*this) + (-o);
}

Polynomial	Polynomial::operator-() const
{
	Polynomial	p(*this);
	for (size_t k = 0; k < p._c.size(); k++)
		p._c[k] = -p._c[k];
	return p;
}

Polynomial	Polynomial::operator*(const Polynomial &o) const
{
	if (isZero() || o.isZero())
		return Polynomial();
	if (degree() + o.degree() > MAX_DEGREE)
		throw std::runtime_error("polynomial degree too large");
	Polynomial	p;
	p._c.assign(_c.size() + o._c.size() - 1, Complex(0));
	for (size_t a = 0; a < _c.size(); a++)
		for (size_t b = 0; b < o._c.size(); b++)
			p._c[a + b] = p._c[a + b] + _c[a] * o._c[b];
	p.trim();
	return p;
}

Polynomial	Polynomial::pow(int n) const
{
	if (n < 0)
		throw std::runtime_error("exponent must be an integer >= 0");
	if (degree() > 0 && static_cast<long>(degree()) * n > MAX_DEGREE)
		throw std::runtime_error("polynomial degree too large");
	Polynomial	result(Complex(1));
	Polynomial	base(*this);
	while (n > 0)
	{
		if (n & 1)
			result = result * base;
		n >>= 1;
		if (n > 0)
			base = base * base;
	}
	return result;
}

bool	Polynomial::divide(const Polynomial &o, Polynomial &quotient) const
{
	if (o.isZero())
		return false;
	Polynomial	rest(*this);
	Polynomial	q;
	if (degree() >= o.degree())
		q._c.assign(degree() - o.degree() + 1, Complex(0));
	while (!rest.isZero() && rest.degree() >= o.degree())
	{
		int	shift = rest.degree() - o.degree();
		Complex	k = rest._c.back() / o._c.back();
		q._c[shift] = k;
		for (int j = 0; j <= o.degree(); j++)
			rest._c[j + shift] = rest._c[j + shift] -  k * o._c[j];
		rest._c.pop_back();
		rest.trim();
	}
	if (!rest.isZero())
		return false;
	q.trim();
	quotient = q;
	return true;
}

static std::string	term(const Complex &c, int k, const std::string &var, bool &negative)
{
	std::string	magnitude;
	if (c.isReal())
	{
		negative = c.re().value() < 0;
		magnitude = (negative ? -c.re() : c.re()).toString();
	}
	else if (c.re().isZero())
	{
		negative = c.im().value() < 0;
		Complex	m(0, negative ? -c.im() : c.im());
		magnitude = m.toString();
	}
	else
	{
		negative = false;
		magnitude = "(" + c.toString() + ")";
	}

	if (k == 0)
		return magnitude;
	std::string	power = var;
	if (k > 1)
	{
		std::ostringstream	ss;
		ss << var << "^" << k;
		power = ss.str();
	}
	if (magnitude == "1")
		return power;
	return magnitude + " * " + power;
}

std::string	Polynomial::toString(const std::string &var) const
{
	if (isZero())
		return "0";
	std::string	out;
	for (int k = degree(); k >= 0; k--)
	{
		if (!_c[k].isZero())
		{
			bool	negative;
			std::string	t = term(_c[k], k, var, negative);
			if (out.empty())
				out = (negative ? "-" : "") + t;
			else
				out += (negative ? " - " : " + ") + t;
		}
	}
	return out;
}