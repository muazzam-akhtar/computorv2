#ifndef COMPUTOR_HPP
#define COMPUTOR_HPP

#include <string>
#include "Complex.hpp"
#include "Parser.hpp"

class Computor
{
	public:
		std::string process(const std::string &);
	private:
		Complex	evaluate(const NodePtr &node);
};

#endif