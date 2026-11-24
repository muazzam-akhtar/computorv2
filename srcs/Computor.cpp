#include "../includes/Computor.hpp"
#include "../includes/Evaluator.hpp"
#include "../includes/Simplifier.hpp"
#include "../includes/Solver.hpp"
#include <stdexcept>

static std::string	listNames(const std::set<std::string> &names)
{
	std::string	list;
	for (std::set<std::string>::const_iterator	it = names.begin(); it != names.end(); ++it)
		list += (list.empty() ? "" : ", ") + *it;
	return list;
}

std::string	Computor::show(const NodePtr &expr) const
{
	Simplifier	simp(_vars);
	
	if (expr->kind() == N_VARIABLE)
	{
		const std::string	&name = static_cast<const VariableNode &>(*expr).name;
		if (_vars.hasFunction(name))
		{
			const Function	&f = _vars.getFunction(name);
			return name + "(" + f.param + ") = " + f.toString();
		}
	}
	if (Simplifier::containsCall(expr))
	{
		std::set<std::string>	unknowns = simp.freeVariables(expr);
		if (unknowns.size() > 1)
		{
			std::string	list;
			for (std::set<std::string>::const_iterator it = unknowns.begin(); it != unknowns.end(); ++it)
				list += (list.empty() ? "" : ", ") + *it;
			throw std::runtime_error("several unknowns (" + list + "): only one is supported");
		}
		if (unknowns.size() == 1)
		{
			const std::string	x = *unknowns.begin();
			Polynomial			p;
			if (!simp.toPolynomial(simp.substitute(expr, x), x, p))
				throw std::runtime_error("cannot simplify this expression with the unknown '" + x + "'");
			return p.toString(x);
		}
	}
	return Evaluator(_vars).evaluate(expr).toString();
}

static bool	sameValue(const Value &a, const Value &b)
{
	if (a.isMatrix() != b.isMatrix())
		return false;
	return a.isMatrix() ? a.matrix() == b.matrix() : a.scalar() == b.scalar();
}

std::string	Computor::solve(const NodePtr &lhs, const NodePtr &rhs) const
{
	Simplifier	simp(_vars);
	std::set<std::string>	unknowns = simp.freeVariables(lhs);
	std::set<std::string>	other = simp.freeVariables(rhs);

	unknowns.insert(other.begin(), other.end());
	if (unknowns.size() > 1)
		throw std::runtime_error("several unknowns (" + listNames(unknowns) + "): only one is supported");
	if (unknowns.empty())
	{
		Evaluator	eval(_vars);
		return sameValue(eval.evaluate(lhs), eval.evaluate(rhs)) ? "true" : "false";
	}

	const std::string	x = *unknowns.begin();
	Polynomial			left, right;
	if (!simp.toPolynomial(simp.substitute(lhs, x), x, left)
		|| !simp.toPolynomial(simp.substitute(rhs, x), x, right))
		throw std::runtime_error("cannot solve: both sides must be polynomials in '" + x + "'");
	
	Polynomial	reduced = left - right;
	Solution	sol = Solver::solve(reduced);
	std::string	out = reduced.toString(x) + " = 0";

	if (sol.kind == Solution::ALL)
		return out + "\nAny number is a solution";
	if (sol.kind == Solution::NONE)
		return out + "\nNo solution";
	out += std::string("\n") + (sol.roots.size() == 1 ? "A solution: " : "Two solutions: ") + (sol.real ? "R" : "C") + " :";
	for (size_t k = 0; k < sol.roots.size(); k++)
		out += "\n" + sol.roots[k].toString();
	return out;
}

std::string Computor::process(const std::string &line)
{
	Statement	s = Parser::parseStatement(line);
	Evaluator	eval(_vars);

	switch (s.kind)
	{
		case S_EVAL:
		case S_QUERY:
			return indent(show(s.lhs));
		case S_ASSIGN_VAR:
		{
			Value	v = eval.evaluate(s.rhs);
			_vars.set(s.name, v);
			return indent(v.toString());
		}
		case S_ASSIGN_FUNC:
		{
			Function	f = Simplifier(_vars).define(s.param, s.rhs);
			_vars.setFunction(s.name, f);
			return indent(f.toString());
		}
		case S_SOLVE:
			return indent(solve(s.lhs, s.rhs));
	}
	throw std::logic_error("unknown statement");
}

const SymbolTable	&Computor::variables() const	{	return _vars;	}

std::string	Computor::indent(const std::string &text)
{
	std::string	out = "  ";
	for (size_t k = 0; k < text.size(); k++)
	{
		out += text[k];
		if (text[k] == '\n' && k + 1 < text.size())
			out += "  ";
	}
	return out;
}
