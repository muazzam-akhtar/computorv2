#include "../includes/Matrix.hpp"
#include <stdexcept>
#include <sstream>
#include <cmath>

Matrix::Matrix(size_t rows, size_t cols) : _rows(rows), _cols(cols), _data(rows * cols, Complex(0))
{
	if (rows == 0 || cols == 0)
		throw std::runtime_error("a matrix needs at least one row and one column");
}

Matrix::Matrix(const std::vector<std::vector<Complex>> &rows) : _rows(rows.size()), _cols(rows.empty() ? 0 : rows[0].size())
{
	if (_rows == 0 || _cols == 0)
		throw std::runtime_error("a matrix needs at least one row and one column");
	for (size_t r = 0; r < _rows; r++)
	{
		if (rows[r].size() != _cols)
			throw std::runtime_error("matrix rows must have all the same length");
		_data.insert(_data.end(), rows[r].begin(), rows[r].end());
	}
}

Matrix	Matrix::identity(size_t n)
{
	Matrix	m(n, n);
	for (size_t k = 0; k < n; k++)
		m.at(k, k) = Complex(1);
	return m;
}

size_t	Matrix::rows() const	{	return _rows;	}
size_t	Matrix::cols() const	{	return _cols;	}
bool	Matrix::isSquare() const	{	return _rows == _cols;	}

const Complex	&Matrix::at(size_t r, size_t c) const
{
	if (r >= _rows || c >= _cols)
		throw std::out_of_range("matrix index out of bounds");
	return _data[r * _cols + c];
}
Complex	&Matrix::at(size_t r, size_t c)	{	return _data[r * _cols + c];	}

std::string	Matrix::sizeString() const
{
	std::ostringstream	ss;
	ss << _rows << "x" << _cols;
	return ss.str();
}

void	Matrix::requireSameSize(const Matrix &o, const char *what) const
{
	if (_rows != o._rows || _cols != o._cols)
		throw std::runtime_error(
			std::string("cannot ") + what + " matrices of different sizes ("
			+ sizeString() + " and " + o.sizeString() + ")");
}

Matrix	Matrix::operator+(const Matrix &o) const
{
	requireSameSize(o, "add");
	Matrix	m(*this);
	for (size_t k = 0; k < _data.size(); k++)
		m._data[k] = _data[k] + o._data[k];
	return m;
}

Matrix	Matrix::operator-(const Matrix &o) const
{
	requireSameSize(o, "subtract");
	Matrix	m(*this);
	for (size_t k = 0; k < _data.size(); k++)
		m._data[k] = _data[k] - o._data[k];
	return m;
}

Matrix	Matrix::operator*(const Matrix &o) const
{
	requireSameSize(o, "multiply term-to-term");
	Matrix	m(*this);
	for (size_t k = 0; k < _data.size(); k++)
		m._data[k] = _data[k] * o._data[k];
	return m;
}

Matrix	Matrix::operator-() const
{
	Matrix	m(*this);
	for (size_t k = 0; k < _data.size(); k++)
		m._data[k] = -_data[k];
	return m;
}

Matrix	Matrix::operator*(const Complex &k) const
{
	Matrix	m(*this);
	for (size_t i = 0; i < _data.size(); i++)
		m._data[i] = _data[i] * k;
	return m;
}

Matrix	Matrix::operator/(const Complex &k) const
{
	if (k.isZero())
		throw std::runtime_error("division by zero");
	Matrix	m(*this);
	for (size_t i = 0; i < _data.size(); i++)
		m._data[i] = _data[i] / k;
	return m;
}

Matrix	Matrix::multiply(const Matrix &o) const
{
	if (_cols != o.rows())
		throw std::runtime_error(
			"cannot multiply " + sizeString() + " ** "
			+ o.sizeString()
			+ ": the left matrix needs as many columns as the right one has rows"
		);
	Matrix	m(_rows, o._cols);
	for (size_t r = 0; r < _rows; r++)
	{
		for (size_t c = 0; c < o._cols; c++)
		{
			Complex	sum(0);
			for (size_t k = 0; k < _cols; k++)
				sum = sum + at(r, k) * o.at(k, c);
			m.at(r, c) = sum;
		}
	}
	return m;
}

Matrix	Matrix::pow(const Complex &e) const
{
	if (!isSquare())
		throw std::runtime_error("only a square matrix can be raised to a power (this one is " + sizeString() + ")");
	if (!e.isReal() || !e.re().isInteger() || e.re().value() < 0)
		throw std::runtime_error("a matrix exponent must be an integer >= 0");
	if (e.re().value() > 1e15)
		throw std::overflow_error("exponent too large");
	unsigned long long	n = static_cast<unsigned long long>(std::llround(e.re().value()));
	Matrix	base(*this);
	Matrix	result = identity(_rows);
	while (n > 0)
	{
		if (n & 1)
			result = result.multiply(base);
		n >>= 1;
		if (n > 0)
			base = base.multiply(base);
	}
	return result;
}

Matrix	Matrix::transpose() const
{
	Matrix	t(_cols, _rows);
	for (size_t r = 0; r < _rows; r++)
		for (size_t c = 0; c < _cols; c++)
			t.at(c, r) = at(r, c);
	return t;
}

static double	magnitude(const Complex &c) {	return std::hypot(c.re().value(), c.im().value());	}

static size_t	pivotRow(const Matrix &m, size_t col, size_t from)
{
	size_t	best = from;
	for (size_t r = from + 1; r < m.rows(); r++)
		if (magnitude(m.at(r, col)) > magnitude(m.at(best, col)))
			best = r;
	return best;
}

static void	swapRows(Matrix &m, size_t a, size_t b)
{
	for (size_t c = 0; c < m.cols(); c++)
	{
		Complex	tmp = m.at(a, c);
		m.at(a, c) = m.at(b, c);
		m.at(b, c) = tmp;
	}
}

Complex	Matrix::determinant() const
{
	if (!isSquare())
		throw std::runtime_error("only a square matrix has a determinant (this one is " + sizeString() + ")");
	Matrix	a(*this);
	Complex	det(1);
	for (size_t col = 0; col < _rows; col++)
	{
		size_t	p = pivotRow(a, col, col);
		if (a.at(p, col).isZero())
			return Complex(0);
		if (p != col)
		{
			swapRows(a, p, col);
			det = -det;
		}
		det = det * a.at(col, col);
		for (size_t r = col + 1; r < _rows; r++)
		{
			Complex	f = a.at(r, col) / a.at(col, col);
			for (size_t c = col; c < _cols; c++)
				a.at(r, c) = a.at(r, c) - f * a.at(col, c);
		}
	}
	return det;
}

Matrix	Matrix::inverse() const
{
	if (!isSquare())
		throw std::runtime_error("only a square matrix can be inverted (this one is " + sizeString() + ")");
	Matrix	a(*this);
	Matrix	inv = identity(_rows);
	for (size_t col = 0; col < _rows; col++)
	{
		size_t	p = pivotRow(a, col, col);
		if (a.at(p, col).isZero())
			throw std::runtime_error("this matrix is singular (determinant 0): it has no inverse");
		if (p != col)
		{
			swapRows(a, p, col);
			swapRows(inv, p, col);
		}
		Complex	pivot = a.at(col, col);
		for (size_t c = 0; c < _cols; c++)
		{
			a.at(col, c) = a.at(col, c) / pivot;
			inv.at(col, c) = inv.at(col, c) / pivot;
		}
		for (size_t r = 0; r < _rows; r++)
		{
			if (r == col)
				continue;
			Complex	f = a.at(r, col);
			if (f.isZero())
				continue;
			for (size_t c = 0; c < _cols; c++)
			{
				a.at(r, c) = a.at(r, c) - f * a.at(col, c);
				inv.at(r, c) = inv.at(r, c) - f * inv.at(col, c);
			}
		}
	}
	return inv;
}

bool	Matrix::operator==(const Matrix &o) const
{
	if (_rows != o._rows || _cols != o._cols)
		return false;
	for (size_t k = 0; k < _data.size(); k++)
		if (_data[k] != o._data[k])
			return false;
	return true;
}

std::string	Matrix::toString() const
{
	std::string	s;
	for (size_t r = 0; r < _rows; r++)
	{
		if (r)
			s += "\n";
		s += "[ ";
		for (size_t c = 0; c < _cols; c++)
		{
			if (c)
				s += " , ";
			s += at(r, c).toString();
		}
		s += " ]";
	}
	return s;
}

std::string	Matrix::toInlineString() const
{
	std::string	s = "[";
	for (size_t r = 0; r < _rows; r++)
	{
		if (r)
			s += ";";
		s += "[";
		for (size_t c = 0; c < _cols; c++)
		{
			if (c)
				s += ",";
			s += at(r, c).toString();
		}
		s += "]";
	}
	return s + "]";
}