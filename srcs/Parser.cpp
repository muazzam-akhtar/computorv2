#include "../includes/Parser.hpp"
#include "../includes/Lexer.hpp"

std::string	Statement::describe() const
{
	switch (kind)
	{
		case S_EVAL:		return "eval " + lhs->toString();
		case S_QUERY:		return "query " + lhs->toString();
		case S_SOLVE:		return "solve " + lhs->toString() + " = " + rhs->toString();
		case S_ASSIGN_VAR:	return "var " + name + " = " + rhs->toString();
		case S_ASSIGN_FUNC:	return "func " + name + "(" + param + ") = " + rhs->toString();
	}
	return "?";
}

NodePtr	Parser::slice(const std::vector<Token> &tokens, size_t from, size_t to)
{
	std::vector<Token>	part(tokens.begin() + from, tokens.begin() + to);
	part.push_back(Token(T_END, "", tokens[to].pos));
	Parser	p(part);
	return p.parseAll();
}

Statement	Parser::parseStatement(const std::string &line)
{
	std::vector<Token>	t = Lexer::tokenize(line);
	size_t				end = t.size() - 1;
	size_t				eq = end;
	size_t				eqCount = 0;
	size_t				question = end;

	for (size_t k = 0; k < end; k++)
	{
		if (t[k].type == T_EQUAL)
		{
			if (++eqCount > 1)
				throw SyntaxError("only one '=' is allowed", t[k].pos);
			eq = k;
		}
		else if (t[k].type == T_QUESTION)
		{
			if (k != end - 1)
				throw SyntaxError("'?' must be the last character", t[k].pos);
			question = k;
		}
	}

	Statement	s;
	if (question != end)
	{
		if (eqCount == 0)
			throw SyntaxError("'?' needs an '=' before it, like: 2 + 2 = ?", t[question].pos);
		s.lhs = slice(t, 0, eq);
		if (eq + 1 == question)
			s.kind = S_QUERY;
		else
		{
			s.kind = S_SOLVE;
			s.rhs = slice(t, eq + 1, question);
		}
		return s;
	}
	if (eqCount == 0)
	{
		s.kind = S_EVAL;
		s.lhs = slice(t, 0, end);
		return s;
	}

	if (eq == 0)
		throw SyntaxError("missing name before '='", t[0].pos);
	if (eq + 1 == end)
		throw SyntaxError("missing value after '='", t[end].pos);
	if (t[0].type == T_IMAG || (eq == 4 && t[2].type == T_IMAG))
		throw SyntaxError(
			"'i' is reserved for the imaginary unit",
			t[0].type == T_IMAG ? t[0].pos : t[2].pos
		);
	if (eq == 1 && t[0].type == T_IDENT)
	{
		s.kind = S_ASSIGN_VAR;
		s.name = t[0].text;
	}
	else if (eq == 4 && t[0].type == T_IDENT && t[1].type == T_LPAREN && t[2].type == T_IDENT && t[3].type == T_RPAREN)
	{
		s.kind = S_ASSIGN_FUNC;
		s.name = t[0].text;
		s.param = t[2].text;
	}
	else
		throw SyntaxError("can only assign to a name (x = ....) or a function (f(x) = ...)", t[0].pos);
	s.rhs = slice(t, eq + 1, end);
	return s;
}

NodePtr	Parser::parseExpression(const std::string &text)
{
	Parser	p(Lexer::tokenize(text));
	return p.parseAll();
}

NodePtr	Parser::parseAll()
{
	if (peek().type == T_END)
		throw SyntaxError("empty expression", peek().pos);
	NodePtr	e = expr();
	if (peek().type != T_END)
		throw unexpected(peek());
	return e;
}

Parser::Parser(const std::vector<Token> &tokens) : _t(tokens), _i(0) {}

NodePtr	Parser::expr()
{
	NodePtr	left = term();
	while (peek().type == T_PLUS || peek().type == T_MINUS)
	{
		std::string	op = peek().text;
		++_i;
		left = NodePtr(new BinaryNode(op, left, term()));
	}
	return left;
}

NodePtr	Parser::term()
{
	NodePtr	left = unary();
	while (true)
	{
		Tokentype	t = peek().type;
		if (t == T_STAR || t == T_DSTAR || t == T_SLASH || t == T_PERCENT)
		{
			std::string	op = peek().text;
			++_i;
			left = NodePtr(new BinaryNode(op, left, unary()));
		}
		else
			return left;
	}
}

NodePtr	Parser::unary()
{
	if (match(T_MINUS))
		return NodePtr(new UnaryNode(unary()));
	if (match(T_PLUS))
		return unary();
	return power();
}

NodePtr	Parser::power()
{
	NodePtr	base = primary();
	Tokentype	next = peek().type;
	if (previous().type == T_NUMBER && (next == T_IDENT || next == T_IMAG || next == T_LPAREN))
		return NodePtr(new BinaryNode("*", base, power()));
	if (match(T_CARET))
		return NodePtr(new BinaryNode("^", base, unary()));
	return base;
}

NodePtr	Parser::primary()
{
	const Token	&tok = peek();
	switch (tok.type)
	{
		case T_NUMBER:
			++_i;
			return NodePtr(new NumberNode(Rational(tok.number)));
		case T_IMAG:
			++_i;
			return NodePtr(new ImaginaryNode());
		case T_IDENT:
		{
			std::string	name = tok.text;
			++_i;
			if (match(T_LPAREN))
			{
				NodePtr	arg = expr();
				expect(T_RPAREN, "')' to close the call to " + name);
				return NodePtr(new CallNode(name, arg));
			}
			if (name == "pi")
				return NodePtr(new ConstantNode(Value(Complex(Rational(M_PI)))));
			return NodePtr(new VariableNode(name));
		}
		case T_LPAREN:
		{
			++_i;
			NodePtr	inner = expr();
			expect(T_RPAREN, "')'");
			return inner;
		}
		case T_LBRACKET:
			++_i;
			return matrix();
		default:
			throw unexpected(tok);
	}
}

NodePtr	Parser::matrix()
{
	size_t	start = previous().pos;
	std::vector<std::vector<NodePtr>>	rows;
	do
	{
		rows.push_back(matrixRow());
		if (rows.back().size() != rows.front().size())
			throw SyntaxError("matrix rows must all have the same length", start);
	}
	while (match(T_SEMICOLON));
	expect(T_RBRACKET, "']' to close the matrix");
	return NodePtr(new MatrixNode(rows));
}

std::vector<NodePtr>	Parser::matrixRow()
{
	expect(T_LBRACKET, "'[' to start a matrix row");
	std::vector<NodePtr>	row;
	do
		row.push_back(expr());
	while (match(T_COMMA));
	expect(T_RBRACKET, "']' to close the matrix row");
	return row;
}

const Token	&Parser::peek() const {	return _t[_i];	}

const Token	&Parser::previous() const
{
	static const Token	none(T_END, "", 0);
	return _i == 0 ? none : _t[_i - 1];
}

bool	Parser::match(Tokentype type)
{
	if (peek().type != type)
		return false;
	++_i;
	return true;
}

void	Parser::expect(Tokentype type, const std::string &what)
{
	if (match(type))
		return;
	if (peek().type == T_END)
		throw SyntaxError("expected " + what + " but the input ended", peek().pos);
	throw SyntaxError("expected " + what + " but found '" + peek().text + "'", peek().pos);
}

SyntaxError	Parser::unexpected(const Token &tok) const
{
	if (tok.type == T_END)
		return SyntaxError("unexpected end of input", tok.pos);
	return SyntaxError("unexpected '" + tok.text + "'", tok.pos);
}