#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include "Complex.hpp"

class Value
{
	public:
		enum Type {	RATIONAL, COMPLEX	};

		Value();
		Value(const Complex &c);

		Type			type() const;
		std::string		typeName() const;
		const Complex	&scalar() const;

		static Value	apply(const std::string &op, const Value &l, const Value &r);
		Value			negate() const;
		std::string		toString() const;
	private:
		Complex	_scalar;
};

#endif