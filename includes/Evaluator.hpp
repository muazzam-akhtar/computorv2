#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "../includes/Ast.hpp"
#include "../includes/Value.hpp"
#include "../includes/SymbolTable.hpp"

class Evaluator
{
	public:
		explicit	Evaluator(const SymbolTable &);
		Value		evaluate(const NodePtr &) const;
		Value		call(const Function &, const Value &) const;
	private:
		Evaluator(const SymbolTable &, const std::string &, const Value &);
		const SymbolTable	&_vars;
		std::string			_param;
		Value				_arg;
		bool				_bound;
};

#endif
