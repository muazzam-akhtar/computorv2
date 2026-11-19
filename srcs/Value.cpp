#include "../includes/Value.hpp"
#include <stdexcept>

Value::Value() : _scalar(0) {}

Value::Value(const Complex &c) : _scalar(c) {}

Value::Type	Value::type() const	{	return _scalar.isReal() ? RATIONAL : COMPLEX;	}

std::string	Value::typeName() const
{
	switch (type())
	{
		case RATIONAL: return "rational";
		case COMPLEX: return "complex";
	}
	return "?";
}

const Complex	&Value::scalar() const	{	return _scalar;	}

Value	Value::negate() const	{	return Value(-_scalar);	}

Value	Value::apply(const std::string &op, const Value &l, const Value &r)
{
	const Complex	&a = l.scalar();
	const Complex	&b = r.scalar();

	if (op == "+")	return Value(a + b);
	if (op == "-")	return Value(a - b);
	if (op == "*")	return Value(a * b);
	if (op == "/")	return Value(a / b);
	if (op == "%")	return Value(a % b);
	if (op == "^")	return Value(a.pow(b));
	if (op == "**")
		throw std::runtime_error(
			"'**' is matrix multiplication, it cannot be used on "
			+ l.typeName() + " and " + r.typeName() + "; use '*'");
	throw std::logic_error("unknown operator '" + op + "'");
}

std::string	Value::toString() const	{	return _scalar.toString();	}
