#ifndef COMPLEX_HPP
#define COMPLEX_HPP
#include <string>
#include <ostream>
#include "Rational.hpp"

class Complex
{
	public:
		Complex();
		Complex(const Rational &);
		Complex(const Rational &, const Rational &);
		static Complex  i();

		const Rational  &re() const;
		const Rational  &im() const;

		bool    isReal() const;
		bool    isZero() const;

		Complex operator+(const Complex &) const;
		Complex operator-(const Complex &) const;
		Complex operator*(const Complex &) const;
		Complex operator/(const Complex &) const;
		Complex operator%(const Complex &) const;
		Complex operator-() const;
		Complex pow(const Complex &) const;
		Complex	sqrt() const;

		bool    operator==(const Complex &) const;
		bool    operator!=(const Complex &) const;

		std::string toString() const;
	
	private:
		Rational    _re;
		Rational    _im;
};

std::ostream    &operator<<(std::ostream &, const Complex &);

#endif