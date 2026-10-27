#include "../includes/Matrix.hpp"

template<typename K>
Matrix<K>::Matrix(const std::vector<std::vector<K>> &m) : _m(m) {}

template<typename K>
Matrix<K>::Matrix(const Matrix &src) { *this = src; }

template<typename K>
Matrix<K>   &Matrix<K>::operator=(const Matrix<K> &src)
{
    if (this != &src)
        this->_m = src._m;
    return (*this);
}

template<typename K>
Matrix<K>::~Matrix() {}

template<typename K>
Matrix<K> Matrix<K>::operator+(const Matrix<K> &o) const
{
    if (_m.size() != o._m.size())
        throw std::invalid_argument("Matrix::+ dimension mismatch");
    std::vector<std::vector<K>> out(_m.size());
    for (std::size_t i = 0; i < _m.size(); ++i)
    {
        if (_m[i].size() != o._m[i].size())
            throw std::invalid_argument("Matrix::+ dimension mismatch");
        out[i].resize(_m[i].size());
        for (std::size_t j = 0; j < _m[i].size(); ++j)
            out[i][j] = _m[i][j] + o._m[i][j];
    }
    return Matrix(out);
}

template<typename K>
Matrix<K> Matrix<K>::operator-(const Matrix<K> &o) const
{
    if (_m.size() != o._m.size())
        throw std::invalid_argument("Matrix::- dimension mismatch");
    std::vector<std::vector<K>> out(_m.size());
    for (std::size_t i = 0; i < _m.size(); ++i)
    {
        if (_m[i].size() != o._m[i].size())
            throw std::invalid_argument("Matrix::- dimension mismatch");
        out[i].resize(_m[i].size());
        for (std::size_t j = 0; j < _m[i].size(); ++j)
            out[i][j] = _m[i][j] - o._m[i][j];
    }
    return Matrix(out);
}

template<typename K>
Matrix<K> Matrix<K>::operator*(const Matrix<K> &o) const
{
    if (_m.empty() || o._m.empty())
        throw std::invalid_argument("Matrix::* empty operand");
    std::size_t n = _m.size();
    std::size_t m = _m[0].size();
    std::size_t p = o._m[0].size();
    if (o._m.size() != m)
        throw std::invalid_argument("Matrix::* incompatible dimensions");
    std::vector<std::vector<K>> out(n, std::vector<K>(p, K()));
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < p; ++j)
        {
            K sum = K();
            for (std::size_t k = 0; k < m; ++k)
                sum = sum + _m[i][k] * o._m[k][j];
            out[i][j] = sum;
        }
    return Matrix(out);
}

template<typename K>
std::string Matrix<K>::toString() const
{
    std::string s = "[";
    for (std::size_t i = 0; i < _m.size(); i++)
    {
        if (i)
            s += ", ";
        s += "[";
        for (std::size_t j = 0; j < _m[i].size(); j++)
        {
            if (j)
                s += ", ";
            s += _m[i][j].toString();
        }
        s += "]";
    }
    s += "]";
    return s;
}

template class Matrix<Rational>;
template class Matrix<Complex>;
