#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP

#include <string>
#include <vector>
#include "Complex.hpp"

class Polynomial
{
	public:
		static const int 				MAX_DEGREE = 1000;

		Polynomial();
		Polynomial(const Complex &);
		static Polynomial				x();
		int								degree() const;
		bool							isZero() const;
		bool							isConstant() const;
		bool							hasRealCoefficients() const;
		const Complex					&coef(int k) const;
		Complex							evaluate(const Complex &) const;

		Polynomial						operator+(const Polynomial &o) const;
		Polynomial						operator-(const Polynomial &o) const;
		Polynomial						operator*(const Polynomial &o) const;
		Polynomial						operator-() const;
		Polynomial						pow(int) const;

		bool							divide(const Polynomial &, Polynomial &) const;
		std::string						toString(const std::string &) const;

	private:
		std::vector<Complex>			_c;
		void							trim();
};

#endif