#include "../includes/Complex.hpp"

Complex::Complex() {}

Complex::Complex(const Rational &r, const Rational &i) : re(r), im(i) {}

Complex::Complex(const Complex &src) { *this = src; }

Complex &Complex::operator=(const Complex &src)
{
    if (this != &src)
    {
        this->re = src.re;
        this->im = src.im;
    }
    return *this;
}

Complex::~Complex() {}

Complex Complex::operator+(const Rational &r) const
{
    return (Complex(re + r, im));
}

Complex Complex::operator-(const Rational &r) const
{
    return (Complex(re - r, im));
}

Complex Complex::operator*(const Rational &r) const
{
    return (Complex(re * r, im * r));
}

Complex Complex::operator/(const Rational &r) const
{
    return (Complex(re / r, im / r));
}

Complex Complex::operator+(const Complex &c) const
{
    return (Complex(re + c.re, im + c.im));
}

Complex Complex::operator-(const Complex &c) const
{
    return (Complex(re - c.re, im - c.im));
}

Complex Complex::operator*(const Complex &c) const
{
    Rational real = re * c.re - im * c.im;
    Rational imag = re * c.im + im * c.re;
    return Complex(real, imag);
}

Complex Complex::operator/(const Complex &c) const
{
    Rational denom = c.re * c.re + c.im * c.im;   // |o|²
    if (denom.isZero())
        throw std::domain_error("Complex: division by zero");
    Rational real = (re * c.re + im * c.im) / denom;
    Rational imag = (im * c.re - re * c.im) / denom;
    return Complex(real, imag);
}


Complex Complex::pow(int e) const
{
    if (e < 0)
        throw std::domain_error("Complex::pow: negative exponent not supported");

    Complex result(Rational(1, 1), Rational(1, 1));
    Complex base(*this);
    while (e > 0)
    {
        if (e & 1)
        {
            Rational    nr = result.re * base.re - result.im * base.im;
            Rational    ni = result.re * base.re + result.im * base.im;
            result = Complex(nr, ni);
        }
        Rational    br = base.re * base.re - base.im * base.im;
        Rational    bi = Rational(2, 1) * base.re * base.im;
        base = Complex(br, bi);
        e >>= 1;
    }
    return result;
}

std::string Complex::toString() const
{
    std::string r = re.toString();
    std::string i = im.toString();
    bool    neg = (i.size() > 0 && i[0] == '-');
    if (im.isZero())
        return r;
    if (neg)
    {
        i = i.substr(1);
        return r + " - " + i + "i";
    }
    return r + " + " + i + "i";
}

bool    Complex::isZero() const { return (re.isZero() && im.isZero()); }