#ifndef COMPUTOR_HPP
#define COMPUTOR_HPP

#include <string>
#include "Parser.hpp"
#include "SymbolTable.hpp"

class Computor
{
	public:
		std::string			process(const std::string &);
		const SymbolTable	&variables() const;
	private:
		SymbolTable			_vars;
		static std::string	indent(const std::string &text);
};

#endif
