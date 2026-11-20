#include "../includes/Value.hpp"
#include <stdexcept>

Value::Value() : _isMatrix(false), _scalar(0), _matrix(1, 1) {}

Value::Value(const Complex &c) : _isMatrix(false), _scalar(c), _matrix(1, 1) {}

Value::Value(const Matrix &m) : _isMatrix(true), _scalar(0), _matrix(m) {}

Value::Type	Value::type() const
{
	if (_isMatrix)
		return MATRIX;
	return _scalar.isReal() ? RATIONAL : COMPLEX;
}

bool	Value::isMatrix() const	{	return _isMatrix;	}

std::string	Value::typeName() const
{
	switch (type())
	{
		case RATIONAL: return "rational";
		case COMPLEX: return "complex";
		case MATRIX: return "matrix";
	}
	return "?";
}

const Complex	&Value::scalar() const
{
	if (_isMatrix)
		throw std::runtime_error("expected a number but got a matrix");
	return _scalar;
}

const Matrix	&Value::matrix() const
{
	if (!_isMatrix)
		throw std::runtime_error("expected a matrix but got a " + typeName());
	return _matrix;
}

Value	Value::negate() const
{
	if (_isMatrix)
		return Value(-_matrix);
	return Value(-_scalar);
}

static std::runtime_error	unsupported(const std::string &op, const Value &l, const Value &r, const std::string &hint)
{
	std::string	msg = "cannot use '" + op + "' between a " + l.typeName() + " and a " + r.typeName();
	if (!hint.empty())
		msg += " (" + hint + ")";
	return std::runtime_error(msg);
}

static Value	numbers(const std::string &op, const Complex &a, const Complex &b, const Value &l, const Value &r)
{
	if (op == "+")	return Value(a + b);
	if (op == "-")	return Value(a - b);
	if (op == "*")	return Value(a * b);
	if (op == "/")	return Value(a / b);
	if (op == "%")	return Value(a % b);
	if (op == "^")	return Value(a.pow(b));
	if (op == "**")	throw unsupported(op, l, r, "'**' is matrix multiplication, use '*' for numbers");
	throw std::logic_error("unknown operator '" + op + "'");
}

static Value	matrices(const std::string &op, const Matrix &a, const Matrix &b, const Value &l, const Value &r)
{
	if (op == "+")	return Value(a + b);
	if (op == "-")	return Value(a - b);
	if (op == "*")	return Value(a * b);
	if (op == "**")	return Value(a.multiply(b));
	if (op == "/")	throw unsupported(op, l, r, "matrix division is not defined");
	if (op == "^")	throw unsupported(op, l, r, "the exponent must be an integer");
	throw unsupported(op, l, r, "");
}

static Value	matrxiNumber(const std::string &op, const Matrix &m, const Complex &k, const Value &l, const Value &r)
{
	if (op == "*")	return Value(m * k);
	if (op == "/")	return Value(m / k);
	if (op == "^")	return Value(m.pow(k));
	if (op == "**")	throw unsupported(op, l, r, "use '*' to scale a matrix");
	if (op  == "+" || op == "-")	throw unsupported(op, l, r, "a number cannot be added to a matrix");
	throw unsupported(op, l, r, "");
}

static Value	numberMatrix(const std::string &op, const Complex &k, const Matrix &m, const Value &l, const Value &r)
{
	if (op == "*")	return Value(m * k);
	if (op == "**")	throw unsupported(op, l, r, "use '*' to scale a matrix");
	if (op  == "+" || op == "-")	throw unsupported(op, l, r, "a number cannot be added to a matrix");
	throw unsupported(op, l, r, "");
}

Value	Value::apply(const std::string &op, const Value &l, const Value &r)
{
	if (!l._isMatrix && !r._isMatrix)	return numbers(op, l._scalar, r._scalar, l, r);
	if (l._isMatrix && r._isMatrix)	return matrices(op, l._matrix, r._matrix, l, r);
	if (l._isMatrix)	return matrxiNumber(op, l._matrix, r._scalar, l, r);
	return numberMatrix(op, l._scalar, r._matrix, l, r);
}

std::string	Value::toString() const	{	return _isMatrix ? _matrix.toString() : _scalar.toString();	}
