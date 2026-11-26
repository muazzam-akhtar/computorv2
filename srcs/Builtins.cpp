#include "../includes/Builtins.hpp"
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <vector>

static const char	*NAMES[] = {	"abs", "sqrt", "exp", "sin", "cos", "tan", "asin", "acos", "atan", "norm", "inv", "det", "transpose", "dot", "cross"	};

static const size_t	COUNT = sizeof(NAMES) / sizeof(NAMES[0]);

bool	Builtins::has(const std::string &name)
{
	for (size_t k = 0; k < COUNT; k++)
		if (name == NAMES[k])
			return true;
	return false;
}

std::vector<std::string>	Builtins::names() {	return std::vector<std::string>(NAMES, NAMES + COUNT);	}

static Rational	checkedNumber(double v)
{
	if (std::isnan(v) || std::isinf(v))
		throw std::overflow_error("result out of range");
	return Rational(v);
}

static double	modulus(const Complex &c)	{	return std::hypot(c.re().value(), c.im().value());	}

static const Complex	&realOnly(const std::string &name, const Value &v)
{
	const Complex	&c = v.scalar();
	if (!c.isReal())
		throw std::runtime_error(name + " is only defined for real numbers (got a complex number)");
	return c;
}

static Value	absolute(const Value &v)
{
	if (v.isMatrix())
		throw std::runtime_error("abs needs a number; use norm(...) for a matrix or a vector");
	return Value(Complex(checkedNumber(modulus(v.scalar()))));
}

static Value	exponential(const Value &v)
{
	const Complex	&c = v.scalar();
	double			mag = std::exp(c.re().value());
	double			arg = c.im().value();

	if (c.isReal())
		return Value(Complex(checkedNumber(mag)));
	return Value(Complex(checkedNumber(mag * std::cos(arg)), checkedNumber(mag * std::sin(arg))));
}

static Value	norm(const Value &v)
{
	if (!v.isMatrix())
		return absolute(v);
	const Matrix	&m = v.matrix();
	double			sum = 0;
	for (size_t r = 0; r < m.rows(); r++)
	{
		for (size_t c = 0; c < m.cols(); c++)
		{
			double	a = modulus(m.at(r, c));
			sum += a * a;
		}
	}
	return Value(Complex(checkedNumber(std::sqrt(sum))));
}

int	Builtins::arity(const std::string &name)	{	return (name == "dot" || name == "cross") ? 2 : 1;	}

static std::vector<Complex>	vectorOf(const std::string &fn, const Value &v)
{
	std::vector<Complex>	out;

	if (!v.isMatrix())
		throw std::runtime_error(fn + " needs two vectors (a matrix with one row or one column), not a number");
	const Matrix	&m = v.matrix();
	if (m.rows() != 1 && m.cols() != 1)
	{
		std::ostringstream	msg;
		msg << fn << " needs vectors (one row or one column), got a " << m.rows() << "x" << m.cols() << " matrix";
		throw std::runtime_error(msg.str());
	}
	for (size_t r = 0; r < m.rows(); r++)
		for (size_t c = 0; c < m.cols(); c++)
			out.push_back(m.at(r, c));
	return out;
}

static Value	dot(const Value &a, const Value &b)
{
	std::vector<Complex>	x = vectorOf("dot", a), y = vectorOf("dot", b);

	if (x.size() != y.size())
	{
		std::ostringstream	msg;
		msg << "dot needs two vectors of the same length (got " << x.size() << " and " << y.size() << ")";
		throw std::runtime_error(msg.str());
	}
	Complex	sum(0);
	for (size_t k = 0; k < x.size(); k++)
		sum = sum + x[k] * y[k];
	return Value(sum);
}

static Value	cross(const Value &a, const Value &b)
{
	std::vector<Complex>	x = vectorOf("cross", a), y = vectorOf("cross", b);

	if (x.size() != 3 || y.size() != 3)
	{
		std::ostringstream	msg;
		msg << "cross needs two vectors of 3 entries (got " << x.size() << " and " << y.size() << ")";
		throw std::runtime_error(msg.str());
	}
	const Matrix	&shape = a.matrix();
	Matrix			out(shape.rows(), shape.cols());
	Complex			z[3] = {	x[1] * y[2] - x[2] * y[1], x[2] * y[0] - x[0] * y[2], x[0] * y[1] - x[1] * y[0]	};
	for (size_t k = 0; k < 3; k++)
		out.at(shape.rows() == 1 ? 0 : k, shape.rows() == 1 ? k : 0) = z[k];
	return Value(out);
}

Value	Builtins::apply(const std::string &name, const Value &a, const Value &b)
{
	if (name == "dot")
		return dot(a, b);
	if (name == "cross")
		return cross(a, b);
	throw std::logic_error(name + " does not take two arguments");
}

Value	Builtins::apply(const std::string &name, const Value &arg)
{
	if (Builtins::arity(name) == 2)
		throw std::runtime_error(name + " takes two arguments");
	if (name == "abs")
		return absolute(arg);
	if (name == "norm")
		return norm(arg);
	if (name == "inv")
		return Value(arg.matrix().inverse());
	if (name == "det")
		return Value(arg.matrix().determinant());
	if (name == "transpose")
		return Value(arg.matrix().transpose());
	if (arg.isMatrix())
		throw std::runtime_error(name + " needs a number, not a matrix");
	if (name == "sqrt")
		return Value(arg.scalar().sqrt());
	if (name == "exp")
		return exponential(arg);
	if (name == "sin")
		return Value(Complex(checkedNumber(std::sin(realOnly(name, arg).re().value()))));
	if (name == "cos")
		return Value(Complex(checkedNumber(std::cos(realOnly(name, arg).re().value()))));
	if (name == "tan")
		return Value(Complex(checkedNumber(std::tan(realOnly(name, arg).re().value()))));
	if (name == "asin")
		return Value(Complex(checkedNumber(std::asin(realOnly(name, arg).re().value()))));
	if (name == "acos")
		return Value(Complex(checkedNumber(std::acos(realOnly(name, arg).re().value()))));
	if (name == "atan")
		return Value(Complex(checkedNumber(std::atan(realOnly(name, arg).re().value()))));
	throw std::logic_error("unknown built-in function " + name);
}
