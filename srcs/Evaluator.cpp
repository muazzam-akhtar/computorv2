#include "../includes/Evaluator.hpp"
#include <stdexcept>

Evaluator::Evaluator(const SymbolTable &vars) : _vars(vars)	{}

Value	Evaluator::evaluate(const NodePtr &node) const
{
	switch (node->kind())
	{
		case N_NUMBER:
			return Value(Complex(static_cast<const NumberNode &>(*node).value));
		case N_IMAGINARY:
			return Value(Complex::i());
		case N_VARIABLE:
			return _vars.get(static_cast<const VariableNode &>(*node).name);
		case N_UNARY:
			return evaluate(static_cast<const UnaryNode &>(*node).operand).negate();
		case N_BINARY:
		{
			const BinaryNode	&b = static_cast<const BinaryNode &>(*node);
			Value				l = evaluate(b.left);
			Value				r = evaluate(b.right);
			return Value::apply(b.op, l, r);
		}
		case N_MATRIX:
			throw std::runtime_error("matrices are not supported yet (S4)");
		case N_CALL:
			throw std::runtime_error("unknown function '" + static_cast<const CallNode &>(*node).name + "'");
	}
	throw std::logic_error("unknown node");
}