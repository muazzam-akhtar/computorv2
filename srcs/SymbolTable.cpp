#include "../includes/SymbolTable.hpp"
#include <stdexcept>


bool	SymbolTable::has(const std::string &name) const	{	return _vars.find(name) != _vars.end();	}

const Value	&SymbolTable::get(const std::string &name) const
{
	std::map<std::string, Value>::const_iterator	it = _vars.find(name);
	if (it == _vars.end())
		throw std::runtime_error("unknown variable '" + name + "'");
	return it->second;
}

void	SymbolTable::set(const std::string &name, const Value &value)	{	_vars[name] = value;	}

void	SymbolTable::remove(const std::string &name)	{	_vars.erase(name);	}

const std::map<std::string, Value>	&SymbolTable::all() const	{	return _vars;	}