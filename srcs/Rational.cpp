#include "../includes/Rational.hpp"

Rational::Rational() : num_(0), den_(1) {}

Rational::Rational(long long n) : num_(n), den_(1) { }

Rational::Rational(long long n, long long d) : num_(n), den_(d)
{
    if (d == 0)
        throw std::invalid_argument("Rational: denominator is 0");
    normalize();
}

Rational::~Rational() {}

Rational::Rational(const Rational &obj) { *this = obj; }

Rational    &Rational::operator=(const Rational &obj)
{
    if (this != &obj)
    {
        this->num_ = obj.num_;
        this->den_ = obj.den_;
    }
    return (*this);
}

void    Rational::normalize()
{
    if (den_ < 0)
    {
        num_ = -num_;
        den_ = -den_;
    }
    long long   gcd = gcd_ll(num_, den_);
    if (gcd == 0)
    {
        num_ = 0;
        den_ = 1;
        return ;
    }
    num_ /= gcd;
    den_ /= gcd;
}

long long   Rational::gcd_ll(long long a, long long b)
{
    if (a < 0)  a = -a;
    if (b < 0)  b = -b;
    while (b)
    {
        long long t = a % b;
        a = b;
        b = t;
    }
    return a;
}

Rational Rational::operator+(const Rational& o) const
{
    return (Rational(num_ * o.den_ + o.num_ * den_, den_ * o.den_));
}

Rational Rational::operator-(const Rational& o) const
{
    return (Rational(num_ * o.den_ - o.num_ * den_, den_ * o.den_));
}

Rational Rational::operator*(const Rational& o) const
{
    return (Rational(num_ * o.num_, den_ * o.den_));
}

Rational Rational::operator/(const Rational& o) const
{
    if (o.num_ == 0)
        throw std::domain_error("Rational: division by 0");
    return (Rational(num_ * o.den_, den_ * o.num_));
}

Rational Rational::operator%(const Rational& o) const
{
    if (o.num_ == 0)
        throw std::domain_error("Rational: modulo by 0");
    long long   common_num = num_ * o.den_;
    long long   common_den = den_ * o.den_;
    long long   divisor_num = o.num_ * den_;
    long long   r = common_num % divisor_num;
    return Rational(r, common_den);
}

Rational    Rational::pow(int e) const
{
    if (e == 0)
        return (Rational(1, 1));
    if (e < 0)
    {
        if (num_ == 0)
            throw std::domain_error("Rational: 0^negative");
        return Rational(den_, num_).pow(-e);
    }
    Rational    result(1, 1);
    Rational    base(*this);
    while (e > 0)
    {
        if (e & 1)
            result = result * base;
        base = base * base;
        e >>= 1;
    }
    return result;
}

std::string Rational::toString() const
{
    if (den_ == 1)  return std::to_string(num_);
    return (std::to_string(num_) + "/" + std::to_string(den_));
}

bool    Rational::isZero() const { return num_ == 0; }
