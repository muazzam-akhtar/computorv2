# ifndef COMPLEX_HPP
# define COMPLEX_HPP

#include <string>
#include "Rational.hpp"

class Complex {
    private:
        Rational    re, im;
    public:
        Complex();
        Complex(const Rational &);
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

        Rational getRe() const;
        Rational getIm() const;
        
        std::string toString() const;
        bool isZero() const;
};

#endif