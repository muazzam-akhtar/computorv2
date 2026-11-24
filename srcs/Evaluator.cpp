#include "../includes/Evaluator.hpp"
#include <stdexcept>

Evaluator::Evaluator(const SymbolTable &vars) : _vars(vars), _bound(false)	{}

Evaluator::Evaluator(const SymbolTable &vars, const std::string &param, const Value &arg) : _vars(vars), _param(param), _arg(arg), _bound(true) {}

Value	Evaluator::call(const Function &f, const Value &arg) const
{
	if (f.isPolynomial && !arg.isMatrix())
		return Value(f.poly.evaluate(arg.scalar()));
	return Evaluator(_vars, f.param, arg).evaluate(f.body);
}

Value	Evaluator::evaluate(const NodePtr &node) const
{
	switch (node->kind())
	{
		case N_NUMBER:
			return Value(Complex(static_cast<const NumberNode &>(*node).value));
		case N_IMAGINARY:
			return Value(Complex::i());
		case N_CONSTANT:
			return static_cast<const ConstantNode &>(*node).value;
		case N_VARIABLE:
		{
			const std::string	&name = static_cast<const VariableNode &>(*node).name;
			if (_bound && name == _param)
				return _arg;
			return _vars.get(name);
		}
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
		{
			const CallNode	&c = static_cast<const CallNode &>(*node);
			const Function	&f = _vars.getFunction(c.name);
			return call(f, evaluate(c.arg));
		}
	}
	throw std::logic_error("unknown node");
}
