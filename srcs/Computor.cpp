#include "../includes/Computor.hpp"
#include <stdexcept>

std::string Computor::process(const std::string &line)
{
	Statement	s = Parser::parseStatement(line);

	switch (s.kind)
	{
		case S_EVAL:
		case S_QUERY:
			return " " + evaluate(s.lhs).toString();
		case S_ASSIGN_VAR:
		case S_ASSIGN_FUNC:
			throw std::runtime_error("assignment is not supported yet (S3)");
		case S_SOLVE:
			throw std::runtime_error("equation solving is not supported yet (S5)");
	}
	throw std::logic_error("unknown statement");
}

Complex	Computor::evaluate(const NodePtr &node)
{
	switch (node->kind())
	{
		case N_NUMBER:
			return Complex(static_cast<const NumberNode &>(*node).value);
		case N_IMAGINARY:
			return Complex::i();
		case N_UNARY:
			return -evaluate(static_cast<const UnaryNode &>(*node).operand);
		case N_BINARY:
		{
			const BinaryNode	&b = static_cast<const BinaryNode &>(*node);
			Complex	l = evaluate(b.left);
			Complex	r = evaluate(b.right);
			if (b.op == "+")	return l + r;
			if (b.op == "-")	return l - r;
			if (b.op == "*")	return l * r;
			if (b.op == "/")	return l / r;
			if (b.op == "%")	return l % r;
			if (b.op == "^")	return l.pow(r);
			if (b.op == "**")
				throw std::runtime_error("'**' is matrix multiplication; use '*' for numbers");
			throw std::logic_error("unknown operator " + b.op);
		}
		case N_VARIABLE:
			throw std::runtime_error("unknown variable '" + static_cast<const VariableNode &>(*node).name + "'");
		case N_CALL:
			throw std::runtime_error("unknown function '" + static_cast<const VariableNode &>(*node).name + "'");
		case N_MATRIX:
			throw std::runtime_error("matrices are not supported yet (S4)");
	}
	throw std::logic_error("unknown node");
}
