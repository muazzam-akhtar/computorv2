#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP

# include <vector>
# include <string>
# include <utility>
# include "Rational.hpp"
# include "Complex.hpp"

/*
 * A polynomial is stored as coefficients from degree 0 upward:
 *   P(x) = c[0] + c[1]*x + c[2]*x^2 + ...
 * Trailing zero coefficients are stripped (canonical form).
 *
 * All functions work for either Rational or Complex coefficients because
 * both types provide: K(), operator+ - * /, pow(int), isZero(), toString().
*/

// ----- canonicalization -----
template<typename K>
void poly_trim(std::vector<K> &c);              // drop trailing zeros

// ----- basic algebra -----
template<typename K>
std::vector<K> poly_add(const std::vector<K> &, const std::vector<K> &);

template<typename K>
std::vector<K> poly_sub(const std::vector<K> &, const std::vector<K> &);

template<typename K>
std::vector<K> poly_mul(const std::vector<K> &, const std::vector<K> &);

template<typename K>
std::vector<K> poly_scale(const std::vector<K> &, const K &);

template<typename K>
std::vector<K> poly_pow(const std::vector<K> &, int);

template<typename K>
K poly_eval(const std::vector<K> &, const K &);

template<typename K>
std::vector<K>  poly_derivative(const std::vector<K> &);

template<typename K>
std::vector<K>  poly_integral(const std::vector<K> &, const K &);

template<typename K>
std::pair<std::vector<K>, std::vector<K>> poly_divmod(const std::vector<K> &, const std::vector<K> &);

// ----- roots / solving -----
// Degree 1: ax + b = 0        -> one root
// Degree 2: ax^2 + bx + c = 0 -> two roots (discriminant may go complex)
// Degree >2: throws std::domain_error (not implemented generically)
// Returns roots as Complex (real solutions have zero imaginary part).
std::vector<Complex>    poly_roots(const std::vector<Rational> &);
std::vector<Complex>    poly_roots(const std::vector<Complex> &);

// ----- pretty print -----
template<typename K>
std::string poly_toString(const std::vector<K> &a, const std::string &var = "x");

#endif