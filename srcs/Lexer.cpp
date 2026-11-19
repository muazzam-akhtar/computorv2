#include "../includes/Lexer.hpp"
#include <cctype>
#include <cstdlib>
#include <sstream>

const char  *tokenName(Tokentype type)
{
    switch (type)
    {
        case T_NUMBER: return "NUMBER";
        case T_IDENT: return "IDENT";
        case T_IMAG: return "IMAG";
        case T_PLUS: return "PLUS";
        case T_MINUS: return "MINUS";
        case T_STAR: return "STAR";
        case T_DSTAR: return "DSTAR";
        case T_SLASH: return "SLASH";
        case T_PERCENT: return "PERCENT";
        case T_CARET: return "CARET";
        case T_LPAREN: return "LPAREN";
        case T_RPAREN: return "RPAREN";
        case T_LBRACKET: return "LBRACKET";
        case T_RBRACKET: return "RBRACKET";
        case T_COMMA: return "COMMA";
        case T_SEMICOLON: return "SEMICOLON";
        case T_EQUAL: return "EQUAL";
        case T_QUESTION: return "QUESTION";
        case T_END: return "END";
    }
    return "?";
}

static std::string  withColumn(const std::string &msg, size_t pos)
{
    std::ostringstream  ss;
    ss << "syntax error at column " << pos + 1 << ": " << msg;
    return ss.str();
}

SyntaxError::SyntaxError(const std::string &msg, size_t pos) : std::runtime_error(withColumn(msg, pos)), _pos(pos) {}

size_t  SyntaxError::pos() const {  return _pos;    }

static bool isDigit(char c) {   return std::isdigit(static_cast<unsigned char>(c)) != 0;    }

static bool isAlpha(char c) {   return std::isalpha(static_cast<unsigned char>(c)) != 0;    }

static bool isSpace(char c) {   return std::isspace(static_cast<unsigned char>(c)) != 0;    }

static Token    readNumber(const std::string &s, size_t &i)
{
    size_t  start = i;
    while (i < s.size() && isDigit(s[i]))
        ++i;
    if (i < s.size() && s[i] == '.')
    {
        ++i;
        if (i >= s.size() || !isDigit(s[i]))
            throw SyntaxError("malformed number '" + s.substr(start, i - start) + "'", start);
        while (i < s.size() && isDigit(s[i]))
            ++i;
    }
    if (i < s.size() && s[i] == '.')
        throw SyntaxError("malformed number '" + s.substr(start, i - start + 1) + "'", start);
    std::string text = s.substr(start, i - start);
    return Token(T_NUMBER, text, start, std::strtod(text.c_str(), nullptr));
}

static Token    readName(const std::string &s, size_t &i)
{
    size_t  start = i;
    while (i < s.size() && isAlpha(s[i]))
        ++i;
    if (i < s.size() && (isDigit(s[i]) || s[i] == '_'))
    {
        size_t  end = i;
        while (end < s.size() && (std::isalnum(static_cast<unsigned char>(s[end])) || s[end] == '_'))
            ++end;
        throw SyntaxError("invalid name '" + s.substr(start, end - start) + "' (names contain letters only)", start);
    }
    std::string name = s.substr(start, i - start);
    for (size_t k = 0; k < name.size(); k++)
        name[k] = static_cast<char>(std::tolower(static_cast<unsigned char>(name[k])));
    return Token(name == "i" ? T_IMAG : T_IDENT, name, start);
}

std::vector<Token>  Lexer::tokenize(const std::string &s)
{
    std::vector<Token>  out;
    size_t              i = 0;

    while (i < s.size())
    {
        char c = s[i];
        if (isSpace(c))
            i++;
        else if (isDigit(c) || (c == '.' && i + 1 < s.size() && isDigit(s[i + 1])))
            out.push_back(readNumber(s, i));
        else if (isAlpha(c))
            out.push_back(readName(s, i));
        else if (c == '*' && i + 1 < s.size() && s[i + 1] == '*')
        {
            out.push_back(Token(T_DSTAR, "**", i));
            i += 2;
        }
        else
        {
            Tokentype   t;
            switch (c)
            {
                case '+': t = T_PLUS;       break;
                case '-': t = T_MINUS;      break;
                case '*': t = T_STAR;       break;
                case '/': t = T_SLASH;      break;
                case '%': t = T_PERCENT;    break;
                case '^': t = T_CARET;      break;
                case '(': t = T_LPAREN;     break;
                case ')': t = T_RPAREN;     break;
                case '[': t = T_LBRACKET;   break;
                case ']': t = T_RBRACKET;   break;
                case ',': t = T_COMMA;      break;
                case ';': t = T_SEMICOLON;  break;
                case '=': t = T_EQUAL;      break;
                case '?': t = T_QUESTION;   break;
                default:
                    throw SyntaxError(std::string("unexpected character '") + c + "'", i);
            }
            out.push_back(Token(t, std::string(1, c), i));
            ++i;
        }
    }
    out.push_back(Token(T_END, "", s.size()));
    return (out);
}

std::string Lexer::dump(const std::vector<Token> &tokens)
{
    std::string out;
    for (size_t k = 0; k < tokens.size(); k++)
    {
        if (k)
            out += " ";
        out += tokenName(tokens[k].type);
        if (tokens[k].type == T_NUMBER || tokens[k].type == T_IDENT)
            out += "(" + tokens[k].text + ")";
    }
    return out;
}