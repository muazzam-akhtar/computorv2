#ifndef LEXER_HPP
#define LEXER_HPP

# include <string>
# include <vector>
# include "Token.hpp"

/*
 * Lexer — turns a raw input line into a flat vector of Tokens.
 *
 * Responsibilities:
 *   - Skip whitespace
 *   - Recognize numbers (integers only for now)
 *   - Recognize identifiers (letters only: [A-Za-z]+)
 *   - Recognize single- and double-character operators
 *       (notably: '**' before '*', so "a ** b" is not "a * (*b)")
 *   - Recognize '[' ']' ';' ',' for matrix literals
 *   - Recognize '=' and '?'
 *   - Always append a Tok::End sentinel at the end of the stream
 *
 * Non-responsibilities (belong to the Parser):
 *   - Precedence, associativity
 *   - Detecting unclosed parentheses (parser does that)
 *   - Case normalization (the Context lowercases variable names, not the Lexer)
 *   - Semantic validation ("i" is reserved, names contain only letters):
 *     the Lexer only guarantees the character class; the Context enforces
 *     the reserved-name rule at definition/use time.
 *
 * Errors:
 *   - Any character not part of the language throws std::invalid_argument
 *     with the 0-based column position and the offending character.
 */

class Lexer
{
    public:
        explicit Lexer(const std::string &src);
        Lexer(const Lexer &) = delete;
        Lexer &operator=(const Lexer &) = delete;
        ~Lexer();

        std::vector<Token> tokenize();
    private:
        std::string     _src;
        std::size_t     _pos;

        // character helpers
        bool    atEnd() const;
        char    peek() const;
        char    peekNext() const;
        char    advance();
        void    skipWhitespace();

        // token builders (return a Token, caller pushes it)
        Token   makeNumber();
        Token   makeIdent();
        Token   makeOperator();

        [[noreturn]] void    fail(const std::string &, std::size_t) const;
};

#endif