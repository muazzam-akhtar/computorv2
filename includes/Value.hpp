#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include "Complex.hpp"
#include "Matrix.hpp"

class Value
{
	public:
		enum Type {	RATIONAL, COMPLEX, MATRIX	};

		Value();
		Value(const Complex &);
		Value(const Matrix &);

		Type			type() const;
		bool			isMatrix() const;
		std::string		typeName() const;
		const Complex	&scalar() const;
		const Matrix	&matrix() const;

		static Value	apply(const std::string &, const Value &, const Value &);
		Value			negate() const;
		std::string		toString() const;
	private:
		bool	_isMatrix;
		Complex	_scalar;
		Matrix	_matrix;
};

#endif