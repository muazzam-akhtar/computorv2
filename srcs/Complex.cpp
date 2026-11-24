#include "../includes/Complex.hpp"
#include <stdexcept>
#include <cmath>
#include <algorithm>

Complex::Complex() : _re(0), _im(0) {}

Complex::Complex(const Rational &r) : _re(r), _im(0) {}

Complex::Complex(const Rational &r, const Rational &im) : _re(r), _im(im) {}

Complex Complex::i() {  return Complex(0, 1);    }

const Rational  &Complex::re() const {  return _re; }

const Rational  &Complex::im() const {  return _im; }

bool    Complex::isReal() const {   return _im.isZero();    }

bool    Complex::isZero() const {   return _re.isZero() && _im.isZero();    }

Complex Complex::operator+(const Complex &c) const
{
    return (Complex(_re + c.re(), _im + c.im()));
}

Complex Complex::operator-(const Complex &c) const
{
    return (Complex(_re - c.re(), _im - c.im()));
}

Complex Complex::operator-() const
{
    return (Complex(-_re, -_im));
}

Complex Complex::operator*(const Complex &c) const
{
    return (
        Complex(
            (_re * c.re()) - (_im * c.im()),
            (_re * c.im() + (_im * c.re()))
        )
    );
}

Complex Complex::operator/(const Complex &c) const
{
    if (c.isZero())
        throw std::runtime_error("division by zero");
    if (c.isReal())
        return (Complex(_re / c.re(), _im / c.re()));
    Rational    den = c.re().pow(2) + c.im().pow(2);
    return (
        Complex(
            ((_re * c.re()) + (_im * c.im())) / den,
            ((_im * c.re()) - (_re * c.im())) / den
        )
    );
}

Complex Complex::operator%(const Complex &c) const
{
    if (!isReal() || !c.isReal())
        throw std::runtime_error("modulo is only defined for real numbers");
    return (Complex(_re % c.re()));
}

Complex Complex::pow(const Complex &e) const
{
    if (!e.isReal())
        throw std::runtime_error("exponent must be a real integer >= 0");
    const Rational &n = e.re();
    if (!n.isInteger() || n.value() < 0)
        throw std::runtime_error("exponent must be a positive integer");
    if (n.value() > 1e15)
        throw std::overflow_error("exponent too large");
    if (isReal())
        return Complex(_re.pow(n));
    unsigned long long  k = static_cast<unsigned long long>(std::llround(n.value()));
    Complex base = *this;
    Complex result(1);
    while (k > 0)
    {
        if (k & 1)
            result = result * base;
        k >>= 1;
        if (k > 0)
            base = base * base;
    }
    return result;
}

Complex	Complex::sqrt() const
{
	double	a = _re.value();
	double	b = _im.value();
	double	r = std::hypot(a, b);
	double	re = std::sqrt(std::max(0.0, (r + a) / 2));
	double	im = std::sqrt(std::max(0.0, (r - a) / 2));
	return Complex(Rational(re), Rational(b < 0 ? -im : im));
}

bool    Complex::operator==(const Complex &c) const {   return _re == c.re() && _im == c.im();  }

bool    Complex::operator!=(const Complex &c) const {   return !(*this == c);    }

std::string Complex::toString() const
{
    if (isReal())
        return _re.toString();
    
    bool    neg = _im.value() < 0;
    Rational    absI = neg ? -_im : _im;
    std::string coef = (absI == Rational(1)) ? "" : absI.toString();

    if (_re.isZero())
        return (neg ? "-" : "") + coef + "i";
    return _re.toString() + (neg ? " - " : " + ") + coef + "i";
}

std::ostream    &operator<<(std::ostream &os, const Complex &src)   {   return os << src.toString();    }