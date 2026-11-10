#include "../includes/Lexer.hpp"
#include <cctype>
#include <stdexcept>

/* ------------------------------------------------------------------ */
/*  Construction / destruction                                        */
/* ------------------------------------------------------------------ */

Lexer::Lexer(const std::string &src) : _src(src), _pos(0) {}

Lexer::~Lexer() {}

/* ------------------------------------------------------------------ */
/*  Public entry point                                                */
/* ------------------------------------------------------------------ */

std::vector<Token> Lexer::tokenize()
{
    _pos = 0;
    std::vector<Token> tokens;
    while (true)
    {
        skipWhitespace();
        if (atEnd())
            break;
        char c = peek();
        if (std::isdigit(static_cast<unsigned char>(c)))
            tokens.push_back(makeNumber());
        else if (std::isalpha(static_cast<unsigned char>(c)))
            tokens.push_back(makeIdent());
        else
            tokens.push_back(makeOperator());
    }
    tokens.push_back(Token{ TokenType::End, "", _pos });
    return tokens;
}

/* ------------------------------------------------------------------ */
/*  Cursor helpers                                                    */
/* ------------------------------------------------------------------ */

bool    Lexer::atEnd() const { return _pos >= _src.size(); }

char    Lexer::peek() const { return atEnd() ? '\0' : _src[_pos]; }

char    Lexer::peekNext() const { return (_pos + 1 >= _src.size()) ? '\0' : _src[_pos + 1]; }

char    Lexer::advance() { return _src[_pos++]; }

void    Lexer::skipWhitespace()
{
    while (!atEnd() && std::isspace(static_cast<unsigned char>(peek())))
        ++_pos;
}

/* ------------------------------------------------------------------ */
/*  Token builders                                                    */
/* ------------------------------------------------------------------ */

/*
 * makeNumber: consumes [0-9]+
 * We only recognize integers at the lexical level. Rationals like "1/2"
 * are lexed as Number("1"), Slash, Number("2") and resolved by the parser.
 * A decimal point '.' is NOT part of the language (subject only shows ints);
 * it will be caught by makeOperator as an invalid character.
*/

Token   Lexer::makeNumber()
{
    std::size_t start = _pos;
    while (!atEnd() && std::isdigit(static_cast<unsigned char>(peek())))
        advance();
    return Token{ TokenType::Number, _src.substr(start, _pos - start), start };
}

/*
 * makeIdentifier: consumes [A-Za-z]+
 * Digits are NOT allowed inside identifiers ("2*xx" is 2 * variable xx;
 * "x2" is therefore invalid — the '2' becomes an unexpected Number after an
 * Ident, and the parser will report it).
 * Case is preserved here; the Context lowercases keys.
*/

Token   Lexer::makeIdent()
{
    std::size_t start = _pos;
    while (!atEnd() && std::isalpha(static_cast<unsigned char>(peek())))
        advance();
    return Token{ TokenType::Ident, _src.substr(start, _pos - start), start };
}

/*
 * makeOperator: handles every non-alphanumeric token.
 * Order matters: '**' must be tested before '*'.
*/

Token   Lexer::makeOperator()
{
    std::size_t start = _pos;
    char    c = advance();
    
    switch (c)
    {
        case '+': return Token{ TokenType::Plus,    "+",    start };
        case '-': return Token{ TokenType::Minus,   "-",    start };
        case '/': return Token{ TokenType::Slash,   "/",    start };
        case '%': return Token{ TokenType::Percent, "%",    start };
        case '^': return Token{ TokenType::Caret,   "^",    start };
        case '(': return Token{ TokenType::LParen,  "(",    start };
        case ')': return Token{ TokenType::RParen,  ")",    start };
        case '[': return Token{ TokenType::LBracket,"[",    start };
        case ']': return Token{ TokenType::RBracket,"]",    start };
        case ',': return Token{ TokenType::Comma,   ",",    start };
        case ';': return Token{ TokenType::Semicolon,";",   start };
        case '=': return Token{ TokenType::Assign,  "=",    start };
        case '?': return Token{ TokenType::Query,   "?",    start };
        case '*':
            if (peek() == '*')
            {
                advance();
                return Token{ TokenType::StarStar, "**", start };
            }
            return Token{ TokenType::Star, "*", start };
    }

    fail(std::string("unexpected character '") + c + "'", start);
}

/* ------------------------------------------------------------------ */
/*  Error reporting                                                   */
/* ------------------------------------------------------------------ */

void    Lexer::fail(const std::string &msg, std::size_t pos) const
{
    throw std::invalid_argument("Lexer: " + msg + " at column " + std::to_string(pos));
}
