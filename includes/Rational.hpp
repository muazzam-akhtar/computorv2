# ifndef RATIONAL_HPP
# define RATIONAL_HPP

#include <string>

class Rational {
    long long num_, den_;
    long long gcd_ll(long long, long long);
    void normalize();
public:
    Rational();
    Rational(long long);
    Rational(long long, long long);
    Rational(const Rational &);
    Rational    &operator=(const Rational &);
    ~Rational(void);
    
    Rational operator+(const Rational&) const;
    Rational operator-(const Rational&) const;
    Rational operator*(const Rational&) const;
    Rational operator/(const Rational&) const;
    Rational operator%(const Rational&) const;
    Rational pow(int e) const;
    std::string toString() const;
    bool isZero() const;
};

#endif