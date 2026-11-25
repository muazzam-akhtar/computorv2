#include "../includes/SymbolTable.hpp"
#include <stdexcept>


bool	SymbolTable::has(const std::string &name) const	{	return _vars.find(name) != _vars.end();	}

const Value	&SymbolTable::get(const std::string &name) const
{
	std::map<std::string, Value>::const_iterator	it = _vars.find(name);
	if (it == _vars.end())
	{
		if (hasFunction(name))
			throw std::runtime_error("'" + name + "' is a function: call it like " + name + "(2)");
		throw std::runtime_error("unknown variable '" + name + "'");
	}
	return it->second;
}

void	SymbolTable::set(const std::string &name, const Value &value)
{
	_vars[name] = value;
	_funcs.erase(name);
}

void	SymbolTable::remove(const std::string &name)	{	_vars.erase(name);	}

const std::map<std::string, Value>	&SymbolTable::all() const	{	return _vars;	}

bool	SymbolTable::hasFunction(const std::string &name) const	{	return _funcs.find(name) != _funcs.end();	}

const Function	&SymbolTable::getFunction(const std::string &name) const
{
	std::map<std::string, Function>::const_iterator	it = _funcs.find(name);
	if (it == _funcs.end())
	{
		if (has(name))
			throw std::runtime_error("'" + name + "' is a variable, not a function");
		throw std::runtime_error("unknown function '" + name + "'");
	}
	return it->second;
}

void	SymbolTable::setFunction(const std::string &name, const Function &f)
{
	_funcs[name] = f;
	_vars.erase(name);
}

const std::map<std::string, Function>	&SymbolTable::functions() const	{	return _funcs;	}