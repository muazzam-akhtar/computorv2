// Unit tests for Lexer, AST printing, Parser and the statement classifier.
// Build & run:  make unit

#include "../includes/Lexer.hpp"
#include "../includes/Parser.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

// ---------------------------------------------------------------- mini framework

static int g_pass = 0;
static int g_fail = 0;

static void report(bool ok, const std::string &what, const std::string &got, const std::string &want, int line)
{
    if (ok)
        ++g_pass;
    else
    {
        ++g_fail;
        std::cout << "  \033[31mFAIL\033[0m line " << line << ": " << what
                  << "\n       got:  \"" << got << "\"\n       want: \"" << want << "\"\n";
    }
}

static std::string tokens(const std::string &s)    { return Lexer::dump(Lexer::tokenize(s)); }
static std::string ast(const std::string &s)       { return Parser::parseExpression(s)->toString(); }
static std::string statement(const std::string &s) { return Parser::parseStatement(s).describe(); }

// Runs f(input), turns any exception into "error: <message>"
#define CHECK(f, input, want) \
    do { std::string _g; try { _g = f(input); } \
         catch (const std::exception &e) { _g = std::string("error: ") + e.what(); } \
         report(_g == (want), #f "(\"" input "\")", _g, (want), __LINE__); } while (0)

// Passes if f(input) throws and the message contains `part`
#define CHECK_ERROR(f, input, part) \
    do { std::string _g = "<no exception>"; bool _ok = false; \
         try { _g = f(input); } \
         catch (const std::exception &e) { _g = e.what(); _ok = _g.find(part) != std::string::npos; } \
         report(_ok, #f "(\"" input "\") should fail", _g, std::string("...") + (part) + "...", __LINE__); } while (0)

static void section(const char *name) { std::cout << "\033[1m" << name << "\033[0m\n"; }

// ---------------------------------------------------------------- Lexer

static void lexerTokens()
{
    section("Lexer: every token type");
    CHECK(tokens, "2 4.242 .5",         "NUMBER(2) NUMBER(4.242) NUMBER(.5) END");
    CHECK(tokens, "varA i x",           "IDENT(vara) IMAG IDENT(x) END");
    CHECK(tokens, "+ - * ** / % ^",     "PLUS MINUS STAR DSTAR SLASH PERCENT CARET END");
    CHECK(tokens, "( ) [ ] , ; = ?",    "LPAREN RPAREN LBRACKET RBRACKET COMMA SEMICOLON EQUAL QUESTION END");
    CHECK(tokens, "",                   "END");
    CHECK(tokens, "   ",                "END");
    CHECK(tokens, "***",                "DSTAR STAR END");
}

static void lexerSubject()
{
    section("Lexer: subject examples, spacing, case");
    CHECK(tokens, "varA = 2*i + 3",     "IDENT(vara) EQUAL NUMBER(2) STAR IMAG PLUS NUMBER(3) END");
    CHECK(tokens, "varB = -4i - 4",     "IDENT(varb) EQUAL MINUS NUMBER(4) IMAG MINUS NUMBER(4) END");
    CHECK(tokens, "varA = [[2,3];[4,3]]",
          "IDENT(vara) EQUAL LBRACKET LBRACKET NUMBER(2) COMMA NUMBER(3) RBRACKET SEMICOLON "
          "LBRACKET NUMBER(4) COMMA NUMBER(3) RBRACKET RBRACKET END");
    CHECK(tokens, "funA(x) = 4x^2",     "IDENT(funa) LPAREN IDENT(x) RPAREN EQUAL NUMBER(4) IDENT(x) CARET NUMBER(2) END");
    CHECK(tokens, "varD    =    2 *(2 + 4 *varC -4 /3)",
          "IDENT(vard) EQUAL NUMBER(2) STAR LPAREN NUMBER(2) PLUS NUMBER(4) STAR IDENT(varc) "
          "MINUS NUMBER(4) SLASH NUMBER(3) RPAREN END");
    CHECK(tokens, "a + 2 = ?",          "IDENT(a) PLUS NUMBER(2) EQUAL QUESTION END");
    CHECK(tokens, "VARA vara VaRa",     "IDENT(vara) IDENT(vara) IDENT(vara) END");
    CHECK(tokens, "I",                  "IMAG END");
    CHECK(tokens, "xx ix in",           "IDENT(xx) IDENT(ix) IDENT(in) END");
    CHECK(tokens, "2*xx",               "NUMBER(2) STAR IDENT(xx) END");
}

static void lexerErrors()
{
    section("Lexer: errors");
    CHECK_ERROR(tokens, "var1 = 2",     "invalid name 'var1'");
    CHECK_ERROR(tokens, "my_var",       "invalid name 'my_var'");
    CHECK_ERROR(tokens, "1.",           "malformed number '1.'");
    CHECK_ERROR(tokens, "1.2.3",        "malformed number");
    CHECK_ERROR(tokens, "2 & 3",        "column 3: unexpected character '&'");
    CHECK_ERROR(tokens, "2 $",          "unexpected character '$'");
    CHECK_ERROR(tokens, "1e5",          "invalid name 'e5'");
}

// ---------------------------------------------------------------- Parser: precedence

static void parserPrecedence()
{
    section("Parser: precedence and associativity");
    CHECK(ast, "2 + 4 *2 - 5 %4 + 2 * (4 + 5)", "2 + 4 * 2 - 5 % 4 + 2 * (4 + 5)");
    CHECK(ast, "1 + 2 * 3",             "1 + 2 * 3");
    CHECK(ast, "(1 + 2) * 3",           "(1 + 2) * 3");
    CHECK(ast, "1 - 2 - 3",             "1 - 2 - 3");          // left-assoc
    CHECK(ast, "1 - (2 - 3)",           "1 - (2 - 3)");
    CHECK(ast, "8 / 4 / 2",             "8 / 4 / 2");
    CHECK(ast, "8 / (4 / 2)",           "8 / (4 / 2)");
    CHECK(ast, "2 * (3 % 4)",           "2 * (3 % 4)");
    CHECK(ast, "2^3^2",                 "2^3^2");              // right-assoc
    CHECK(ast, "(2^3)^2",               "(2^3)^2");
    CHECK(ast, "-2^2",                  "-2^2");               // -(2^2)
    CHECK(ast, "(-2)^2",                "(-2)^2");
    CHECK(ast, "2^-1",                  "2^(-1)");
    CHECK(ast, "-(1 + 2)",              "-(1 + 2)");
    CHECK(ast, "--2",                   "-(-2)");
    CHECK(ast, "+2",                    "2");                  // unary + dropped
    CHECK(ast, "((((1))))",             "1");
    CHECK(ast, "a ** b * c",            "a ** b * c");
    CHECK(ast, "2 * x^5 + 4",           "2 * x^5 + 4");
}

static void parserImplicit()
{
    section("Parser: implicit multiplication");
    CHECK(ast, "4x",                    "4 * x");
    CHECK(ast, "4x^2",                  "4 * x^2");
    CHECK(ast, "2i",                    "2 * i");
    CHECK(ast, "-2i^2",                 "-(2 * i^2)");
    CHECK(ast, "3(x + 1)",              "3 * (x + 1)");
    CHECK(ast, "5 % 2i",                "5 % (2 * i)");        // 2i is one factor
    CHECK(ast, "1 / 2x",                "1 / (2 * x)");
    CHECK(ast, "2 * xx",                "2 * xx");             // xx is a variable, not x*x
    CHECK(ast, "4x + 5 - 2",            "4 * x + 5 - 2");
    CHECK_ERROR(ast, "x y",             "unexpected 'y'");     // only after a number
    CHECK_ERROR(ast, "3 4",             "unexpected '4'");
}

static void parserAtoms()
{
    section("Parser: names, calls, matrices");
    CHECK(ast, "varA",                  "vara");
    CHECK(ast, "funA(2)",               "funa(2)");
    CHECK(ast, "funA(varC)",            "funa(varc)");
    CHECK(ast, "funA(2) + funB(4)",     "funa(2) + funb(4)");
    CHECK(ast, "funA(funB(x))",         "funa(funb(x))");
    CHECK(ast, "funA(x + 1)",           "funa(x + 1)");
    CHECK(ast, "[[1,2];[3,4]]",         "[[1,2];[3,4]]");
    CHECK(ast, "[[3,4]]",               "[[3,4]]");
    CHECK(ast, "[[1];[2];[3]]",         "[[1];[2];[3]]");
    CHECK(ast, "[[1 + 1, varA * 2]]",   "[[1 + 1,vara * 2]]");
    CHECK(ast, "2 * [[1,2]]",           "2 * [[1,2]]");
}

static void parserErrors()
{
    section("Parser: syntax errors");
    CHECK_ERROR(ast, "",                "empty expression");
    CHECK_ERROR(ast, "(1 + 2",          "expected ')' but the input ended");
    CHECK_ERROR(ast, "1 + 2)",          "column 6: unexpected ')'");
    CHECK_ERROR(ast, "1 +",             "unexpected end of input");
    CHECK_ERROR(ast, "* 2",             "column 1: unexpected '*'");
    CHECK_ERROR(ast, "2 * / 3",         "unexpected '/'");
    CHECK_ERROR(ast, "()",              "unexpected ')'");
    CHECK_ERROR(ast, "funA(",           "unexpected end of input");
    CHECK_ERROR(ast, "funA(2",          "')' to close the call to funa");
    CHECK_ERROR(ast, "[[1,2];[3]]",     "same length");
    CHECK_ERROR(ast, "[1,2]",           "'[' to start a matrix row");
    CHECK_ERROR(ast, "[[1,2]",          "']' to close the matrix");
    CHECK_ERROR(ast, "[[1,2;[3,4]]",    "']' to close the matrix row");
    CHECK_ERROR(ast, "[[]]",            "unexpected ']'");
    CHECK_ERROR(ast, "2 = 3",           "unexpected '='");
}

// ---------------------------------------------------------------- statements

static void statementSubject()
{
    section("Statements: every subject example");
    CHECK(statement, "varA = 2",                        "var vara = 2");
    CHECK(statement, "varB = 4.242",                    "var varb = 4.242");
    CHECK(statement, "varC = -4.3",                     "var varc = -4.3");
    CHECK(statement, "varA = 2*i + 3",                  "var vara = 2 * i + 3");
    CHECK(statement, "varB = -4i - 4",                  "var varb = -(4 * i) - 4");
    CHECK(statement, "varA = [[2,3];[4,3]]",            "var vara = [[2,3];[4,3]]");
    CHECK(statement, "varB = [[3,4]]",                  "var varb = [[3,4]]");
    CHECK(statement, "funA(x) = 2*x^5 + 4x^2 - 5*x + 4","func funa(x) = 2 * x^5 + 4 * x^2 - 5 * x + 4");
    CHECK(statement, "funB(y) = 43 * y / (4 % 2 * y)",  "func funb(y) = 43 * y / (4 % 2 * y)");
    CHECK(statement, "funC(z) = -2 * z - 5",            "func func(z) = -2 * z - 5");
    CHECK(statement, "y = x",                           "var y = x");
    CHECK(statement, "y = 2 * i - 4",                   "var y = 2 * i - 4");
    CHECK(statement, "varA = 2 + 4 *2 - 5 %4 + 2 * (4 + 5)", "var vara = 2 + 4 * 2 - 5 % 4 + 2 * (4 + 5)");
    CHECK(statement, "funA(x) = varA + varB * 4 - 1 / 2 + x", "func funa(x) = vara + varb * 4 - 1 / 2 + x");
    CHECK(statement, "varD = funA(varC)",               "var vard = funa(varc)");
    CHECK(statement, "a + 2 = ?",                       "query a + 2");
    CHECK(statement, "funB(x) = 4 -5 + (x + 2)^2 - 4",  "func funb(x) = 4 - 5 + (x + 2)^2 - 4");
    CHECK(statement, "funC(x) = 4x + 5 - 2",            "func func(x) = 4 * x + 5 - 2");
    CHECK(statement, "funA(2) + funB(4) = ?",           "query funa(2) + funb(4)");
    CHECK(statement, "funC(3) = ?",                     "query func(3)");
    CHECK(statement, "funA(x) = y ?",                   "solve funa(x) = y");
    CHECK(statement, "varB= 2 * (4 + varA + 3)",        "var varb = 2 * (4 + vara + 3)");
    CHECK(statement, "varC =2 * varB",                  "var varc = 2 * varb");
    CHECK(statement, "varD    =    2 *(2 + 4 *varC -4 /3)", "var vard = 2 * (2 + 4 * varc - 4 / 3)");
    CHECK(statement, "matA = [[1,2];[3,2];[3,4]]",      "var mata = [[1,2];[3,2];[3,4]]");
    CHECK(statement, "matB=   [[1,2]]",                 "var matb = [[1,2]]");
    CHECK(statement, "funA(b) = 2*b+b",                 "func funa(b) = 2 * b + b");
    CHECK(statement, "funB(a)   =2 * a",                "func funb(a) = 2 * a");
    CHECK(statement, "funC(y) =2* y + 4 -2 * 4+1/3",    "func func(y) = 2 * y + 4 - 2 * 4 + 1 / 3");
    CHECK(statement, "funD(x)    =    2 *x",            "func fund(x) = 2 * x");
    CHECK(statement, "funA(funB(x)) = ?",               "query funa(funb(x))");
    CHECK(statement, "x = 2 * xx",                      "var x = 2 * xx");
}

static void statementKinds()
{
    section("Statements: classification");
    CHECK(statement, "2 + 2",                           "eval 2 + 2");
    CHECK(statement, "varA = ?",                        "query vara");
    CHECK(statement, "funA(x) = ?",                     "query funa(x)");
    CHECK(statement, "x^2 + 2x + 1 = 0 ?",              "solve x^2 + 2 * x + 1 = 0");
    CHECK(statement, "a=?",                             "query a");
    CHECK(statement, "a = b",                           "var a = b");
}

static void statementErrors()
{
    section("Statements: errors");
    CHECK_ERROR(statement, "i = 2",                     "'i' is reserved");
    CHECK_ERROR(statement, "f(i) = 2",                  "'i' is reserved");
    CHECK_ERROR(statement, "= 2",                       "missing name before '='");
    CHECK_ERROR(statement, "x =",                       "missing value after '='");
    CHECK_ERROR(statement, "a = b = 2",                 "only one '='");
    CHECK_ERROR(statement, "2 = 3",                     "can only assign");
    CHECK_ERROR(statement, "x + 1 = 3",                 "can only assign");
    CHECK_ERROR(statement, "f(x + 1) = 3",              "can only assign");
    CHECK_ERROR(statement, "f(2) = 3",                  "can only assign");
    CHECK_ERROR(statement, "2 + 2 ?",                   "needs an '='");
    CHECK_ERROR(statement, "a ? = 2",                   "must be the last");
    CHECK_ERROR(statement, "a = ? ?",                   "must be the last");
    CHECK_ERROR(statement, "= ?",                       "empty expression");
    CHECK_ERROR(statement, "?",                         "needs an '='");
    CHECK_ERROR(statement, "x = 2 +",                   "unexpected end of input");
    CHECK_ERROR(statement, "var1 = 2",                  "invalid name");
}

// ---------------------------------------------------------------- main

int main()
{
    lexerTokens();
    lexerSubject();
    lexerErrors();
    parserPrecedence();
    parserImplicit();
    parserAtoms();
    parserErrors();
    statementSubject();
    statementKinds();
    statementErrors();

    std::cout << "\n" << (g_fail ? "\033[31m" : "\033[32m")
              << g_pass << " passed, " << g_fail << " failed\033[0m" << std::endl;
    return g_fail ? 1 : 0;
}