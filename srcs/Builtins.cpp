#include "../includes/Builtins.hpp"
#include <cmath>
#include <stdexcept>

static const char	*NAMES[] = {	"abs", "sqrt", "exp", "sin", "cos", "tan", "asin", "acos", "atan", "norm", "inv", "det", "transpose"	};

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

Value	Builtins::apply(const std::string &name, const Value &arg)
{
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
