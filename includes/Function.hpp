#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <string>
#include "Ast.hpp"
#include "Polynomial.hpp"

struct Function
{
	std::string	param;
	NodePtr		body;
	bool		isPolynomial;
	Polynomial	poly;
	std::string	toString() const {	return (isPolynomial ? poly.toString(param) : body->toString());	}
};

#endif