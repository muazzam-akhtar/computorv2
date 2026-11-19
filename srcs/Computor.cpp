#include "../includes/Computor.hpp"
#include "../includes/Evaluator.hpp"
#include <stdexcept>

std::string Computor::process(const std::string &line)
{
	Statement	s = Parser::parseStatement(line);
	Evaluator	eval(_vars);

	switch (s.kind)
	{
		case S_EVAL:
		case S_QUERY:
			return indent(eval.evaluate(s.lhs).toString());
		case S_ASSIGN_VAR:
		{
			Value	v = eval.evaluate(s.rhs);
			_vars.set(s.name, v);
			return indent(v.toString());
		}
		case S_ASSIGN_FUNC:
			throw std::runtime_error("assignment is not supported yet (S3)");
		case S_SOLVE:
			throw std::runtime_error("equation solving is not supported yet (S5)");
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
