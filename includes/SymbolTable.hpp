#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>
#include <map>
#include "Value.hpp"

class SymbolTable
{
	public:
		bool								has(const std::string &) const;
		const Value							&get(const std::string &) const;
		void								set(const std::string &, const Value &);
		void								remove(const std::string &);
		const std::map<std::string, Value>	&all() const;
	private:
		std::map<std::string, Value>		_vars;
};

#endif