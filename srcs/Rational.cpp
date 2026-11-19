#include "../includes/Rational.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

static const double EPS = 1e-10;
static const int MAX_DECIMALS = 9;
static const int SIGNIFICANT  = 15;

static bool nearlyEqual(double a, double b)
{
    double  scale = std::max(1.0, std::max(std::fabs(a), std::fabs(b)));
    return std::fabs(a - b) <= EPS * scale;
}

Rational::Rational() : _v(0) {}

Rational::Rational(double v) : _v(v) {}

double  Rational::value() const {   return _v;  }

bool    Rational::isZero() const {  return nearlyEqual(_v, 0.0);    }

bool    Rational::isInteger() const {   return nearlyEqual(_v, std::round(_v)); }

Rational    Rational::checked(double v)
{
    if (std::isnan(v) || std::isinf(v))
        throw std::overflow_error("result out of range");
    return Rational(v);
}

Rational    Rational::operator+(const Rational &o) const {  return checked(_v + o._v);  }
Rational    Rational::operator-(const Rational &o) const {  return checked(_v - o._v);  }
Rational    Rational::operator*(const Rational &o) const {  return checked(_v * o._v);  }
Rational    Rational::operator-() const {  return Rational(-_v);  }
Rational    Rational::operator/(const Rational &o) const
{
    if (o.isZero())
        throw std::runtime_error("division by zero");
    return checked(_v / o._v);
}

Rational    Rational::operator%(const Rational &o) const
{
    if (o.isZero())
        throw std::runtime_error("division by zero");
    return checked(std::fmod(_v, o._v));
}

Rational    Rational::pow(const Rational &e) const
{
    if (!e.isInteger() || e._v < 0)
        throw std::runtime_error("exponent must be an integer >= 0");
    if (e._v > 1e15)
        throw std::overflow_error("exponent too large");
    unsigned long long n = static_cast<unsigned long long>(std::llround(e._v));
    double  base = _v;
    double  result = 1.0;
    while (n > 0)
    {
        if (n & 1)
            result *= base;
        n >>= 1;
        if (n > 0)
            base *= base;
    }
    return checked(result);
}

bool    Rational::operator==(const Rational &o) const   {   return (nearlyEqual(_v, o._v)); }
bool    Rational::operator!=(const Rational &o) const   {   return (!(*this == o)); }

std::string Rational::toString() const
{
    std::ostringstream  ss;
    double              a = std::fabs(_v);
    if (isInteger() && a < 1e15)
    {
        ss << std::llround(_v);
        return ss.str();
    }
    if (a >= 1e15)
    {
        ss << std::setprecision(SIGNIFICANT) << _v;
        return ss.str();
    }
    int intDigits = (a < 1.0) ? 1 : static_cast<int>(std::floor(std::log10(a))) + 1;
    int decimals  = std::max(0, std::min(MAX_DECIMALS, SIGNIFICANT - intDigits));

    ss << std::fixed << std::setprecision(decimals) << _v;
    std::string s = ss.str();
    if (s.find('.') != std::string::npos)
    {
        s.erase(s.find_last_not_of('0') + 1);
        if (s[s.size() - 1] == '.')
            s.erase(s.size() - 1);
    }
    if (s == "-0")
        s = "0";
    return s;
}

std::ostream    &operator<<(std::ostream &os, const Rational &r)
{
    return os << r.toString();
}
