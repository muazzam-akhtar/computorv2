#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType
{
    Number, Ident,
    Plus, Minus, Star, Slash, Percent, Caret, StarStar,
    LParen, RParen, LBracket, RBracket,
    Comma, Semicolon,
    Assign,
    Query,
    End,
};

struct Token
{
    TokenType       kind;
    std::string     text;
    std::size_t     pos;
};

#endif