#include "../includes/Simplifier.hpp"
#include "../includes/Evaluator.hpp"
#include <stdexcept>

Simplifier::Simplifier(const SymbolTable &table) : _table(table) {}

NodePtr	Simplifier::replace(const NodePtr &n, const std::string &name, const NodePtr &with)
{
	switch (n->kind())
	{
		case N_NUMBER:
		case N_IMAGINARY:
		case N_CONSTANT:
			return n;
		case N_VARIABLE:
			return (static_cast<const VariableNode &>(*n).name == name ? with : n);
		case N_UNARY:
			return NodePtr(new UnaryNode(replace(static_cast<const UnaryNode &>(*n).operand, name, with)));
		case N_BINARY:
		{
			const BinaryNode	&b = static_cast<const BinaryNode &>(*n);
			return NodePtr(new BinaryNode(b.op, replace(b.left, name, with), replace(b.right, name, with)));
		}
		case N_MATRIX:
		{
			const MatrixNode	&m = static_cast<const MatrixNode &>(*n);
			std::vector<std::vector<NodePtr>>	rows;
			for (size_t r = 0; r < m.rows.size(); r++)
			{
				rows.push_back(std::vector<NodePtr>());
				for (size_t c = 0; c < m.rows[r].size(); c++)
					rows[r].push_back(replace(m.rows[r][c], name, with));
			}
			return NodePtr(new MatrixNode(rows));
		}
		case N_CALL:
		{
			const CallNode	&c = static_cast<const CallNode &>(*n);
			return NodePtr(new CallNode(c.name, replace(c.arg, name, with)));
		}
	}
	throw std::logic_error("unknown node");
}

bool	Simplifier::mentions(const NodePtr &n, const std::string &name)
{
	switch (n->kind())
	{
		case N_VARIABLE:
			return static_cast<const VariableNode &>(*n).name == name;
		case N_UNARY:
			return mentions(static_cast<const UnaryNode &>(*n).operand, name);
		case N_BINARY:
			return mentions(static_cast<const BinaryNode &>(*n).left, name) || mentions(static_cast<const BinaryNode &>(*n).right, name);
		case N_CALL:
			return mentions(static_cast<const CallNode &>(*n).arg, name);
		case N_MATRIX:
		{
			const MatrixNode	&m = static_cast<const MatrixNode &>(*n);
			for (size_t r = 0; r < m.rows.size(); r++)
				for (size_t c = 0; c < m.rows[r].size(); c++)
					if (mentions(m.rows[r][c], name))
						return true;
			return false;
		}
		default:
			return false;
	}
}

bool	Simplifier::containsCall(const NodePtr &n)
{
	switch (n->kind())
	{
		case N_CALL:
			return true;
		case N_UNARY:
			return containsCall(static_cast<const UnaryNode &>(*n).operand);
		case N_BINARY:
			return containsCall(static_cast<const BinaryNode &>(*n).left) || containsCall(static_cast<const BinaryNode &>(*n).right);
		case N_MATRIX:
		{
			const MatrixNode	&m = static_cast<const MatrixNode &>(*n);
			for (size_t r = 0; r < m.rows.size(); r++)
				for (size_t c = 0; c < m.rows[r].size(); c++)
					if (containsCall(m.rows[r][c]))
						return true;
			return false;
		}
		default:
			return false;
	}
}

static void	collectNames(const NodePtr &n, const SymbolTable &t, std::set<std::string> &out)
{
	switch (n->kind())
	{
		case N_VARIABLE:
		{
			const std::string	&name = static_cast<const VariableNode &>(*n).name;
			if (!t.has(name) && !t.hasFunction(name))
				out.insert(name);
			return ;
		}
		case N_UNARY:
			collectNames(static_cast<const UnaryNode &>(*n).operand, t, out);
			return ;
		case N_BINARY:
			collectNames(static_cast<const BinaryNode &>(*n).left, t, out);
			collectNames(static_cast<const BinaryNode &>(*n).right, t, out);
			return ;
		case N_CALL:
			collectNames(static_cast<const CallNode &>(*n).arg, t, out);
			return ;
		case N_MATRIX:
		{
			const MatrixNode	&m = static_cast<const MatrixNode &>(*n);
			for (size_t r = 0; r < m.rows.size(); r++)
				for (size_t c = 0; c < m.rows[r].size(); c++)
					collectNames(m.rows[r][c], t, out);
			return ;
		}
		default:
			return ;
	}
}

std::set<std::string>	Simplifier::freeVariables(const NodePtr &n) const
{
	std::set<std::string>	out;
	collectNames(n, _table, out);
	return out;
}

NodePtr	Simplifier::substitute(const NodePtr &n, const std::string &keep) const
{
	switch (n->kind())
	{
		case N_NUMBER:
		case N_IMAGINARY:
		case N_CONSTANT:
			return n;
		case N_VARIABLE:
		{
			const std::string	&name = static_cast<const VariableNode &>(*n).name;
			if (name == keep)
				return n;
			return NodePtr(new ConstantNode(_table.get(name)));
		}
		case N_UNARY:
			return NodePtr(new UnaryNode(substitute(static_cast<const UnaryNode &>(*n).operand, keep)));
		case N_BINARY:
		{
			const BinaryNode	&b = static_cast<const BinaryNode &>(*n);
			return NodePtr(new BinaryNode(b.op, substitute(b.left, keep), substitute(b.right, keep)));
		}
		case N_MATRIX:
		{
			const MatrixNode	&m = static_cast<const MatrixNode &>(*n);
			std::vector<std::vector<NodePtr>>	rows;
			for (size_t r = 0; r < m.rows.size(); r++)
			{
				rows.push_back(std::vector<NodePtr>());
				for (size_t c = 0; c < m.rows[r].size(); c++)
					rows[r].push_back(substitute(m.rows[r][c], keep));
			}
			return NodePtr(new MatrixNode(rows));
		}
		case N_CALL:
		{
			const CallNode	&c = static_cast<const CallNode &>(*n);
			const Function	&f = _table.getFunction(c.name);
			return replace(f.body, f.param, substitute(c.arg, keep));
		}
	}
	throw std::logic_error("unknown node");
}

static bool	asCount(const Polynomial &p, int &out)
{
	if (!p.isConstant())
		return false;
	const Complex	&e = p.coef(0);
	if (!e.isReal() || !e.re().isInteger() || e.re().value() < 0)
		return false;
	if (e.re().value() > Polynomial::MAX_DEGREE)
		throw std::runtime_error("polynomial degree too large");
	out = static_cast<int>(e.re().value() + 0.5);
	return true;
}

bool	Simplifier::toPolynomial(const NodePtr &n, const std::string &var, Polynomial &out) const
{
	switch (n->kind())
	{
		case N_NUMBER:
			out = Polynomial(Complex(static_cast<const NumberNode &>(*n).value));
			return true;
		case N_IMAGINARY:
			out = Polynomial(Complex::i());
			return true;
		case N_CONSTANT:
		{
			const Value	&v = static_cast<const ConstantNode &>(*n).value;
			if (v.isMatrix())
				return false;
			out = Polynomial(v.scalar());
			return true;
		}
		case N_VARIABLE:
			if (static_cast<const VariableNode &>(*n).name != var)
				return false;
			out = Polynomial::x();
			return true;
		case N_UNARY:
		{
			Polynomial	p;
			if (!toPolynomial(static_cast<const UnaryNode &>(*n).operand, var, p))
				return false;
			out = -p;
			return true;
		}
		case N_BINARY:
		{
			const BinaryNode	&b = static_cast<const BinaryNode &>(*n);
			Polynomial			l, r;
			if (!toPolynomial(b.left, var, l) || !toPolynomial(b.right, var, r))
				return false;
			if (b.op == "+")	{	out = l + r;	return true;	}
			if (b.op == "-")	{	out = l - r;	return true;	}
			if (b.op == "*")	{	out = l * r;	return true;	}
			if (b.op == "^")
			{
				int	k;
				if (!asCount(r, k))
					return false;
				if (l.isConstant())
					out = Polynomial(l.coef(0).pow(Complex(Rational(k))));
				else
					out = l.pow(k);
				return true;
			}
			if (b.op == "/")
			{
				if (r.isZero())
					return false;
				if (r.isConstant())
				{
					out = l * Polynomial(Complex(1) / r.coef(0));
					return true;
				}
				Polynomial	q;
				if (!l.divide(r, q))
					return false;
				out = q;
				return true;
			}
			if (b.op == "%")
			{
				if (!l.isConstant() || !r.isConstant())
					return false;
				out = Polynomial(l.coef(0) % r.coef(0));
				return true;
			}
			return false;
		}
		default:
			return false;
	}
}

Function	Simplifier::define(const std::string &param, const NodePtr &body) const
{
	Function	f;

	f.param = param;
	f.body = substitute(body, param);
	try
	{
		Polynomial	p;
		if (toPolynomial(f.body, param, p))
		{
			f.poly = p;
			f.isPolynomial = true;
		}
	}
	catch (const std::runtime_error &)
	{
		f.isPolynomial = false;
	}
	if (!f.isPolynomial && !mentions(f.body, param))
		Evaluator(_table).evaluate(f.body);
	return f;
}
