#ifndef SIMPLIFIER_HPP
#define SIMPLIFIER_HPP

#include <set>
#include <string>
#include "Ast.hpp"
#include "Function.hpp"
#include "Polynomial.hpp"
#include "SymbolTable.hpp"

class Simplifier
{
	public:
		explicit				Simplifier(const SymbolTable &);
		Function				define(const std::string &, const NodePtr &) const;
		NodePtr					substitute(const NodePtr &, const std::string &) const;
		bool					toPolynomial(const NodePtr &, const std::string &, Polynomial &) const;
		std::set<std::string>	freeVariables(const NodePtr &) const;
		static bool				containsCall(const NodePtr &);
		static bool				mentions(const NodePtr &, const std::string &);
		static NodePtr			replace(const NodePtr &, const std::string &, const NodePtr &);

	private:
		const SymbolTable		&_table;
};

#endif
