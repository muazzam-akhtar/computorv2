#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <string>
#include <ostream>

class Rational
{
    public:
        Rational();
        Rational(double);

        double  value() const;
        bool    isZero() const;
        bool    isInteger() const;

        Rational    operator+(const Rational &o) const;
        Rational    operator-(const Rational &o) const;
        Rational    operator*(const Rational &o) const;
        Rational    operator/(const Rational &o) const;
        Rational    operator%(const Rational &o) const;
        Rational    operator-() const;
        Rational    pow(const Rational &exp) const;

        bool    operator==(const Rational &o) const;
        bool    operator!=(const Rational &o) const;

        std::string toString() const;
    private:
        double _v;
        static Rational checked(double);
};

std::ostream    &operator<<(std::ostream &, const Rational &);

#endif