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
	private:
		const SymbolTable	&_vars;
};

#endif