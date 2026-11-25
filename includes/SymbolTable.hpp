#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>
#include <map>
#include "Value.hpp"
#include "Function.hpp"

class SymbolTable
{
	public:
		bool									has(const std::string &) const;
		const Value								&get(const std::string &) const;
		void									set(const std::string &, const Value &);
		void									remove(const std::string &);
		const std::map<std::string, Value>		&all() const;
		bool									hasFunction(const std::string &) const;
		const Function							&getFunction(const std::string &) const;
		void									setFunction(const std::string &, const Function &);
		const std::map<std::string, Function>	&functions() const;
	private:
		std::map<std::string, Value>			_vars;
		std::map<std::string, Function>			_funcs;
};

#endif