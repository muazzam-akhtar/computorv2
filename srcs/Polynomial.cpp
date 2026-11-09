#include "../includes/Polynomial.hpp"
#include <stdexcept>
#include <sstream>

/* ---------------- canonicalization ---------------- */
template<typename K>
void poly_trim(std::vector<K> &c)
{
    while (!c.empty() && c.back().isZero())
        c.pop_back();
}

/* ---------------- algebra ---------------- */
template<typename K>
std::vector<K> poly_add(const std::vector<K> &a, const std::vector<K> &b)
{
    std::vector<K>  out(std::max(a.size(), b.size()), K());
    for (std::size_t i = 0; i < a.size(); ++i)
        out[i] = out[i] + a[i];
    for (std::size_t i = 0; i < b.size(); ++i)
        out[i] = out[i] + b[i];
    poly_trim(out);
    return out;
}

template<typename K>
std::vector<K> poly_sub(const std::vector<K> &a, const std::vector<K> &b)
{
    std::vector<K>  out(std::max(a.size(), b.size()), K());
    for (std::size_t i = 0; i < a.size(); ++i)
        out[i] = out[i] + a[i];
    for (std::size_t i = 0; i < b.size(); ++i)
        out[i] = out[i] - b[i];
    poly_trim(out);
    return out;
}

template<typename K>
std::vector<K> poly_mul(const std::vector<K> &a, const std::vector<K> &b)
{
    if (a.empty() || b.empty())
        return std::vector<K>();
    std::vector<K>  out(a.size() + b.size() -1, K());
    for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t j = 0; j < b.size(); ++j)
            out[i+j] = out[i+j] + a[i] * b[j];
    poly_trim(out);
    return out;
}

template<typename K>
std::vector<K> poly_scale(const std::vector<K> &a, const K &s)
{
    std::vector<K>  out(a.size(), K());
    for (std::size_t i = 0; i < a.size(); ++i)
        out[i] = a[i] * s;
    poly_trim(out);
    return out;
}

template<typename K>
std::vector<K> poly_pow(const std::vector<K> &a, int e)
{
    if (e < 0)
        throw std::domain_error("Polynomial: negative exponent");

    std::vector<K> out(1);
    if (!out.empty())
        out[0] = out[0] + K();

    if (!a.empty() && !a[0].isZero())
        out[0] = a[0] / a[0];
    else if (a.size() > 1 && !a[1].isZero())
        out[0] = a[1] / a[1];
    else
    {
        if (e == 0)
            throw std::domain_error("poly_pow: 0^0 undefined");
        return std::vector<K>();
    }

    std::vector<K> base(a);
    while (e > 0)
    {
        if (e & 1) out = poly_mul(out, base);
        base = poly_mul(base, base);
        e >>= 1;
    }
    poly_trim(out);
    return out;
}

/* ---------------- evaluation / calculus ---------------- */
template<typename K>
K poly_eval(const std::vector<K> &a, const K &x)
{
    K acc = K();
    for (std::size_t i = a.size(); i-- > 0;)
        acc = acc * x + a[i];
    return acc;
}

template<typename K>
std::vector<K> poly_derivative(const std::vector<K> &a)
{
    if (a.size() < 2)
        return std::vector<K>();

    std::vector<K> out(a.size() - 1, K());
    for (std::size_t i = 1; i < a.size(); ++i)
    {
        K coeff = K();
        for (std::size_t k = 0; k < i; ++k)
            coeff = coeff + a[i];
        out[i - 1] = coeff;
    }
    poly_trim(out);
    return out;
}

template<typename K>
std::vector<K> poly_integral(const std::vector<K> &a, const K &c)
{
    std::vector<K>  out(a.size() + 1, K());
    out[0] = c;
    for (std::size_t i = 0; i < a.size(); ++i)
        out[i + 1] = a[i] / K(Rational(static_cast<long long>(i + 1), 1));
    poly_trim(out);
    return out;
}

/* ---------------- division ---------------- */
template<typename K>
std::pair<std::vector<K>, std::vector<K>> poly_divmod(const std::vector<K> &num, const std::vector<K> &den)
{
    if (den.empty())
        throw std::domain_error("Polynomial: division by 0");
    std::vector<K>  q, r(num);
    poly_trim(r);
    while (r.size() >= den.size() && !r.empty())
    {
        std::size_t shift = r.size() - den.size();
        K factor = r.back() / den.back();
        std::vector<K>  term(shift + 1, K());
        term[shift] = factor;
        q = poly_add(q, term);
        r = poly_sub(r, poly_mul(term, den));
    }
    poly_trim(q);
    poly_trim(r);
    return std::make_pair(q, r);
}

/* ---------------- roots ---------------- */
static Complex  rationalToComplex(const Rational &r)
{
    return Complex(r, Rational(0, 1));
}

std::vector<Complex>    poly_roots(const std::vector<Rational> &coeffs)
{
    std::vector<Complex>    c;
    c.reserve(coeffs.size());
    for (std::size_t i = 0; i < coeffs.size(); i++)
        c.push_back(rationalToComplex(coeffs[i]));
    return poly_roots(c);
}

std::vector<Complex>    poly_roots(const std::vector<Complex> &coeffs)
{
    std::vector<Complex>    c(coeffs);
    poly_trim(c);
    std::vector<Complex>    roots;
    if (c.size() < 1)
        return roots;
    if (c.size() == 2)
    {
        roots.push_back(Complex(Rational(0, 1), Rational(0, 1)) - c[0] / c[1]);
        return roots;
    }
    if (c.size() == 3)
    {
        Complex a = c[2];
        Complex b = c[1];
        Complex cc = c[0];
        Complex disc = Complex(b.getRe() * b.getRe() - a.getRe() * cc.getRe() * Rational(4, 1), Rational(0, 1));
        disc = Complex(b.getRe() * b.getRe() - b.getIm() * b.getIm() - Rational(4, 1) * (a.getRe() * cc.getRe() - a.getIm() * cc.getIm()), Rational(2, 1) * b.getRe() * b.getIm() - Rational(4, 1) * (a.getRe() * cc.getIm() + a.getIm() * cc.getRe()));

        if (!disc.getIm().isZero())
            throw std::domain_error("poly_roots: complex discriminant, add a Complex::sqrt helper first");

        // Rational    d = disc.getRe();
        // long long    dn = 0, dd = 1;
        throw std::domain_error("poly_roots: implement sqrt for full support");

        // (void)a; (void)b; (void)cc; (void)d; (void)dn; (void)dd;
    }
    throw std::domain_error("poly_roots: degree > 2 not implemented");
}

/* ---------------- pretty print ---------------- */

template<typename K>
std::string poly_toString(const std::vector<K> &a, const std::string &var)
{
    std::ostringstream os;
    bool first = true;

    for (std::size_t i = a.size(); i-- > 0;)
    {
        const K &coef = a[i];
        if (coef.isZero())
            continue;

        std::string s = coef.toString();
        bool neg = (s.size() && s[0] == '-');
        std::string mag = neg ? s.substr(1) : s;

        bool unit = (mag == "1");

        if (first)
        {
            if (neg) os << '-';
            first = false;
        }
        else
        {
            os << (neg ? " - " : " + ");
        }

        if (!(unit && i > 0))
            os << mag;

        if (i >= 2)      os << var << "^" << i;
        else if (i == 1) os << var;
    }

    if (first)
        return "0";
    return os.str();
}

/* ---------------- explicit instantiations ---------------- */
template void poly_trim<Rational>(std::vector<Rational> &);
template void poly_trim<Complex>(std::vector<Complex> &);

template std::vector<Rational> poly_add<Rational>(const std::vector<Rational> &, const std::vector<Rational> &);
template std::vector<Complex> poly_add<Complex>(const std::vector<Complex> &, const std::vector<Complex> &);

template std::vector<Rational> poly_sub<Rational>(const std::vector<Rational> &, const std::vector<Rational> &);
template std::vector<Complex> poly_sub<Complex>(const std::vector<Complex> &, const std::vector<Complex> &);

template std::vector<Rational> poly_mul<Rational>(const std::vector<Rational> &, const std::vector<Rational> &);
template std::vector<Complex> poly_mul<Complex>(const std::vector<Complex> &, const std::vector<Complex> &);

template std::vector<Rational> poly_scale<Rational>(const std::vector<Rational> &, const Rational &);
template std::vector<Complex> poly_scale<Complex>(const std::vector<Complex> &, const Complex &);

template std::vector<Rational> poly_pow<Rational>(const std::vector<Rational> &, int);
template std::vector<Complex> poly_pow<Complex>(const std::vector<Complex> &, int);

template Rational poly_eval<Rational>(const std::vector<Rational> &, const Rational &);
template Complex poly_eval<Complex>(const std::vector<Complex> &, const Complex &);

template std::vector<Rational> poly_derivative<Rational>(const std::vector<Rational> &);
template std::vector<Complex> poly_derivative<Complex>(const std::vector<Complex> &);

template std::vector<Rational> poly_integral<Rational>(const std::vector<Rational> &, const Rational &);
template std::vector<Complex> poly_integral<Complex>(const std::vector<Complex> &, const Complex &);

template std::pair<std::vector<Rational>, std::vector<Rational>> poly_divmod<Rational>(const std::vector<Rational> &, const std::vector<Rational> &);
template std::pair<std::vector<Complex>, std::vector<Complex>> poly_divmod<Complex>(const std::vector<Complex> &, const std::vector<Complex> &);

template std::string poly_toString<Rational>(const std::vector<Rational> &, const std::string &);
template std::string poly_toString<Complex>(const std::vector<Complex> &, const std::string &);
