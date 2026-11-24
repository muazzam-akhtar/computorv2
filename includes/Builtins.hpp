#ifndef BUILTINS_HPP
#define BUILTINS_HPP

#include <string>
#include <vector>
#include "Value.hpp"

class Builtins
{
	public:
		static bool							has(const std::string &);
		static Value						apply(const std::string &, const Value &);
		static std::vector<std::string>		names();
};

#endif