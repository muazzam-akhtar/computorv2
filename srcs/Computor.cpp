#include "../includes/Computor.hpp"
#include "../includes/Evaluator.hpp"
#include "../includes/Simplifier.hpp"
#include <stdexcept>

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
			throw std::runtime_error("equation solving is not supported yet (S6)");
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
