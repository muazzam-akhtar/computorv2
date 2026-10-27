# ifndef COMPLEX_HPP
# define COMPLEX_HPP

#include <string>
#include "Rational.hpp"

class Complex {
    Rational    re, im;
public:
    Complex();
    Complex(const Rational &, const Rational &);
    Complex(const Complex &);
    Complex    &operator=(const Complex &);
    ~Complex(void);

    Complex operator+(const Rational&) const;
    Complex operator-(const Rational&) const;
    Complex operator*(const Rational&) const;
    Complex operator/(const Rational&) const;
    Complex operator+(const Complex &) const;
    Complex operator-(const Complex &) const;
    Complex operator*(const Complex &) const;
    Complex operator/(const Complex &) const;
    Complex pow(int e) const;
    std::string toString() const;
    bool isZero() const;
};

#endif