#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <string>
#include <stdexcept>

enum Tokentype
{
    T_NUMBER,
    T_IDENT,
    T_IMAG,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_DSTAR,
    T_SLASH,
    T_PERCENT,
    T_CARET,
    T_LPAREN,
    T_RPAREN,
    T_LBRACKET,
    T_RBRACKET,
    T_COMMA,
    T_SEMICOLON,
    T_EQUAL,
    T_QUESTION,
    T_END
};

struct Token
{
    Tokentype   type;
    std::string text;
    double      number;
    size_t      pos;
    Token(Tokentype t, const std::string &txt, size_t p, double n = 0) : type(t), text(txt), number(n), pos(p) {}
};

const char  *tokenName(Tokentype type);

class SyntaxError : public std::runtime_error
{
    public:
        SyntaxError(const std::string &, size_t);
        size_t  pos() const;
    private:
        size_t  _pos;
};

#endif
