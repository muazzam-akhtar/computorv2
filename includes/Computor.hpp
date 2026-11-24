#ifndef COMPUTOR_HPP
#define COMPUTOR_HPP

#include <string>
#include "Parser.hpp"
#include "SymbolTable.hpp"

class Computor
{
	public:
		std::string											process(const std::string &);
		const SymbolTable									&variables() const;
	private:
		SymbolTable											_vars;
		std::vector<std::pair<std::string, std::string>>	_history;
		std::string											run(const std::string &);
		std::string											listVariables() const;
		std::string											listHistory() const;
		std::string											show(const NodePtr &) const;
		std::string											solve(const NodePtr &, const NodePtr &) const;
		static std::string									indent(const std::string &text);
};

#endif
