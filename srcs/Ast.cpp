#include "../includes/Ast.hpp"

static std::string	paren(const std::string &s) {	return "(" + s + ")";	}

std::string	NumberNode::toString() const {	return value.toString();	}

std::string	UnaryNode::toString() const
{
	std::string	inner = operand->toString();
	if (operand->precedence() < PREC_UNARY || operand->kind() == N_UNARY)
		inner = paren(inner);
	return "-" + inner;
}

int	BinaryNode::precedence() const
{
	if (op  == "+" || op == "-")
		return PREC_ADD;
	if (op == "^")
		return PREC_POW;
	return PREC_MUL;
}

std::string	BinaryNode::toString() const
{
	int			p = precedence();
	std::string	l = left->toString();
	std::string	r = right->toString();

	if (left->precedence() < p || (op == "^" && left->precedence() <= p))
		l = paren(l);
	if (right->precedence() < p || (right->precedence() == p && op != "^"))
		r = paren(r);
	if (op == "^")
		return l + "^" + r;
	return l + " " + op + " " + r;
}

std::string	MatrixNode::toString() const
{
	std::string	s = "[";
	for (size_t i = 0; i < rows.size(); i++)
	{
		if (i)
			s += ";";
		s += "[";
		for (size_t j = 0; j < rows[i].size(); j++)
		{
			if (j)
				s += ",";
			s += rows[i][j]->toString();
		}
		s += "]";
	}
	return s + "]";
}

int	ConstantNode::precedence() const
{
	if (value.isMatrix())
		return PREC_ATOM;
	const Complex	&c = value.scalar();
	if (!c.isReal() && !c.re().isZero())
		return PREC_ADD;
	if (c.re().value() < 0 || (c.re().isZero() && c.im().value() < 0))
		return PREC_UNARY;
	return PREC_ATOM;
}

std::string	ConstantNode::toString() const
{
	if (value.isMatrix())
		return value.matrix().toInlineString();
	return value.toString();
}