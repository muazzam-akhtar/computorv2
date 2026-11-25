#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include "Token.hpp"
#include "Ast.hpp"

enum StatementKind
{
	S_EVAL,
	S_QUERY,
	S_SOLVE,
	S_ASSIGN_VAR,
	S_ASSIGN_FUNC
};

struct Statement
{
	StatementKind	kind;
	std::string		name;
	std::string		param;
	NodePtr			lhs;
	NodePtr			rhs;
	std::string		describe() const;
};

class Parser
{
	public:
		static Statement	parseStatement(const std::string &line);
		static NodePtr		parseExpression(const std::string &text);
	private:
		explicit 				Parser(const std::vector<Token> &tokens);
		std::vector<Token>		_t;
		size_t					_i;
		NodePtr					parseAll();
		NodePtr					expr();
		NodePtr					term();
		NodePtr					unary();
		NodePtr					power();
		NodePtr					primary();
		NodePtr					matrix();
		std::vector<NodePtr>	matrixRow();

		const Token				&peek() const;
		const Token				&previous() const;
		bool					match(Tokentype);
		void					expect(Tokentype, const std::string &);
		SyntaxError				unexpected(const Token &) const;

		static NodePtr			slice(const std::vector<Token> &, size_t, size_t);
};

#endif