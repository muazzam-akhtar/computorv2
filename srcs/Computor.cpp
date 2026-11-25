#include "../includes/Computor.hpp"
#include "../includes/Evaluator.hpp"
#include "../includes/Simplifier.hpp"
#include "../includes/Solver.hpp"
#include "../includes/Builtins.hpp"
#include "../includes/Plotter.hpp"
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <map>

static std::string	listNames(const std::set<std::string> &names)
{
	std::string	list;
	for (std::set<std::string>::const_iterator	it = names.begin(); it != names.end(); ++it)
		list += (list.empty() ? "" : ", ") + *it;
	return list;
}

static void	checkNotReserved(const std::string &name)
{
	if (Builtins::has(name))
		throw std::runtime_error("'" + name + "' is a built-in function and cannot be redefined");
	if (name == "vars" || name == "history" || name == "plot")
		throw std::runtime_error("'" + name + "' is a command and cannot be used as a name");
}

static std::string	trimmed(const std::string &s)
{
	size_t	b = s.find_first_not_of(" \t\r\n");
	if (b == std::string::npos)
		return "";
	return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

static std::string	lowered(std::string s)
{
	for (size_t k = 0; k < s.size(); k++)
		s[k] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[k])));
	return s;
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
	std::set<std::string>	unknowns = simp.freeVariables(expr);
	if (Simplifier::containsCall(expr) || (unknowns.size() == 1 && expr->kind() != N_VARIABLE))
	{
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
			NodePtr				inlined = simp.substitute(expr, x);
			if (!simp.toPolynomial(inlined, x, p))
				return inlined->toString();
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

std::string	Computor::listVariables() const
{
	std::string	out;
	const std::map<std::string, Value>	&vars = _vars.all();
	const std::map<std::string, Function>	&funcs = _vars.functions();

	for (std::map<std::string, Value>::const_iterator it = vars.begin(); it != vars.end(); ++it)
		out += (out.empty() ? "" : "\n") + it->first + " = " + (it->second.isMatrix() ? it->second.matrix().toInlineString() : it->second.toString());
	for (std::map<std::string, Function>::const_iterator it = funcs.begin(); it != funcs.end(); ++it)
		out += (out.empty() ? "" : "\n") + it->first + "(" + it->second.param + ") = " + it->second.toString();
	return out.empty() ? "No variable or function stored yet" : out;
}

std::string	Computor::listHistory() const
{
	if (_history.empty())
		return "History is empty";
	std::string	out;
	for (size_t k = 0; k < _history.size(); ++k)
	{
		std::ostringstream	num;
		num << (k + 1) << ". ";
		out += (k ? "\n" : "") + num.str() + _history[k].first + "\n" + indent(_history[k].second);
	}
	return out;
}

static double	parseBound(const std::string &text)
{
	char	*end = 0;
	double	v = std::strtod(text.c_str(), &end);
	if (text.empty() || *end != '\0' || v!= v || std::fabs(v) > 1e6)
		throw std::runtime_error("'" + text + "' is not a valid range bound (use plain numbers, e.g. plot f -5 5)");
	return v;
}

std::string	Computor::plot(const std::string &args) const
{
	std::istringstream	in(args);
	std::string			name, a, b, extra;
	double				lo = -10, hi = 10;

	in >> name >> a >> b >> extra;
	if (name.empty() || (!a.empty() && b.empty()) || !extra.empty())
		throw std::runtime_error("usage: plot <function> [min max]");
	if (!a.empty())
	{
		lo = parseBound(a);
		hi = parseBound(b);
	}
	if (lo >= hi)
		throw std::runtime_error("the range must satisfy min < max");
	return Plotter::plot(Evaluator(_vars), _vars.getFunction(lowered(name)), lo, hi);
}

std::string Computor::process(const std::string &line)
{
	const std::string	cmd = lowered(trimmed(line));

	if (cmd == "vars")
		return indent(listVariables());
	if (cmd == "history")
		return indent(listHistory());
	if ((cmd == "plot" || cmd.compare(0, 5, "plot ") == 0) && cmd.find('=') == std::string::npos)
	{
		std::string	drawn = indent(plot(cmd.substr(4)));
		_history.push_back(std::make_pair(trimmed(line), drawn));
		return drawn;
	}
	std::string	result = run(line);
	_history.push_back(std::make_pair(trimmed(line), result));
	return result;
}

std::string	Computor::run(const std::string &line)
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
			checkNotReserved(s.name);
			Value	v = eval.evaluate(s.rhs);
			_vars.set(s.name, v);
			return indent(v.toString());
		}
		case S_ASSIGN_FUNC:
		{
			checkNotReserved(s.name);
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
