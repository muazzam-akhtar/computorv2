# ifndef MATRIX_HPP
# define MATRIX_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include "Rational.hpp"
#include "Complex.hpp"

template<typename K>
class Matrix
{
    private:
        std::vector<std::vector<K>> _m;
    public:
        Matrix(const std::vector<std::vector<K>> &);
        Matrix(const Matrix &);
        Matrix  &operator=(const Matrix &);
        ~Matrix(void);
        Matrix operator+(const Matrix&) const;
        Matrix operator-(const Matrix&) const;
        Matrix operator*(const Matrix&) const;
        std::string toString() const;
};

#endif