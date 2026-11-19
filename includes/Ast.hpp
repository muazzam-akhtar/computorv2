#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include "Rational.hpp"

class Node;
typedef std::shared_ptr<const Node> NodePtr;

enum NodeKind { N_NUMBER, N_IMAGINARY, N_VARIABLE, N_UNARY, N_BINARY, N_MATRIX, N_CALL };

enum Precedence { PREC_ADD = 1, PREC_MUL = 2, PREC_UNARY = 3, PREC_POW = 4, PREC_ATOM = 5   };

class Node
{
	public:
		virtual ~Node() {}
		virtual NodeKind	kind() const = 0;
		virtual int			precedence() const {	return PREC_ATOM;	}
		virtual std::string	toString() const = 0;
};

class NumberNode : public Node
{
	public:
		explicit		NumberNode(const Rational &v) : value(v) {}
		NodeKind		kind() const {  return N_NUMBER; }
		std::string		toString() const;
		const Rational	value;
};

class ImaginaryNode : public Node
{
	public:
		NodeKind		kind() const {	return N_IMAGINARY;	}
		std::string		toString() const {	return "i";	}
};

class VariableNode : public Node
{
	public:
		explicit			VariableNode(const std::string &n) : name(n) {}
		NodeKind			kind() const {	return N_VARIABLE;	}
		std::string			toString() const	{	return name;	}
		const std::string	name;
};

class UnaryNode : public Node
{
	public:
		explicit 		UnaryNode(const NodePtr &o) : operand(o) {}
		NodeKind		kind() const {	return N_UNARY;	}
		int				precedence() const {	return PREC_UNARY;	}
		std::string		toString() const;
		const NodePtr	operand;
};

class BinaryNode : public Node
{
	public:
		BinaryNode(const std::string &o, const NodePtr &l, const NodePtr &r) : op(o), left(l), right(r) {}
		NodeKind			kind() const {	return N_BINARY;	}
		int					precedence() const;
		std::string			toString() const;
		const std::string	op;
		const NodePtr		left;
		const NodePtr		right;
};

class MatrixNode : public Node
{
	public:
		explicit MatrixNode(const std::vector<std::vector<NodePtr>> &r) : rows(r) {}
		NodeKind								kind() const {  return N_MATRIX;    }
		std::string								toString() const;
		const std::vector<std::vector<NodePtr>>	rows;
};

class CallNode : public Node
{
	public:
		CallNode(const std::string &n, const NodePtr &a) : name(n), arg(a) {}
		NodeKind			kind() const {	return N_CALL;	}
		std::string			toString() const {	return name + "(" + arg->toString() + ")";	}
		const std::string	name;
		const NodePtr		arg;
};

#endif