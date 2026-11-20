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
		{
			const MatrixNode	&m = static_cast<const MatrixNode &>(*node);
			std::vector<std::vector<Complex>>	rows(m.rows.size());
			for (size_t r = 0; r < m.rows.size(); r++)
			{
				for (size_t c = 0; c < m.rows[r].size(); c++)
				{
					Value	entry = evaluate(m.rows[r][c]);
					if (entry.isMatrix())
						throw std::runtime_error("a matrix entry must be a number, not a matrix");
					rows[r].push_back(entry.scalar());
				}
			}
			return Value(Matrix(rows));
		}
		case N_CALL:
			throw std::runtime_error("unknown function '" + static_cast<const CallNode &>(*node).name + "'");
	}
	throw std::logic_error("unknown node");
}