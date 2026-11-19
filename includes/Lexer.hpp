#ifndef LEXER_HPP
#define LEXER_HPP
#include <string>
#include <vector>
#include "Token.hpp"

class Lexer
{
	public:
		static std::vector<Token>	tokenize(const std::string &);
		static std::string			dump(const std::vector<Token> &);
};

#endif