// Unit tests for functions (S5): definition, simplification, calls, composition.
// Build & run:  make unit

#include "../includes/Computor.hpp"
#include "../includes/Simplifier.hpp"
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

static std::string run(Computor &c, const std::string &line)
{
    try { return c.process(line); }
    catch (const std::exception &e) { return std::string("error: ") + e.what(); }
}

#define EXPECT(c, line, want) \
    do { std::string _g = run(c, line); if (_g.compare(0, 2, "  ") == 0) _g = _g.substr(2); \
         report(_g == (want), "\"" line "\"", _g, (want), __LINE__); } while (0)

#define EXPECT_ERROR(c, line, part) \
    do { std::string _g = run(c, line); \
         bool _ok = _g.compare(0, 7, "error: ") == 0 && _g.find(part) != std::string::npos; \
         report(_ok, "\"" line "\" should fail", _g, std::string("error: ...") + (part) + "...", __LINE__); } while (0)

#define EXPECT_TRUE(cond) \
    report((cond), #cond, (cond) ? "true" : "false", "true", __LINE__)

static void section(const char *name) { std::cout << "\033[1m" << name << "\033[0m\n"; }

// ---------------------------------------------------------------- subject pages

static void subjectAssignment()
{
    section("Subject p7: function definitions");
    Computor c;
    EXPECT(c, "funA(x) = 2*x^5 + 4x^2 - 5*x + 4", "2 * x^5 + 4 * x^2 - 5 * x + 4");
    EXPECT(c, "funB(y) = 43 * y / (4 % 2 * y)",   "43 * y / (4 % 2 * y)");   // cannot be simplified: shown as typed
    EXPECT(c, "funC(z) = -2 * z - 5",             "-2 * z - 5");
}

static void subjectPage8()
{
    section("Subject p8: variables inside functions");
    Computor c;
    EXPECT(c, "varA = 2 + 4 *2 - 5 %4 + 2 * (4 + 5)", "27");
    EXPECT(c, "varB = 2 * varA - 5 %4",               "53");
    EXPECT(c, "funA(x) = varA + varB * 4 - 1 / 2 + x", "x + 238.5");         // known variables are folded in
    EXPECT(c, "varC = 2 * varA - varB",               "1");
    EXPECT(c, "varD = funA(varC)",                    "239.5");
}

static void subjectPage9()
{
    section("Subject p9: image computation");
    Computor c;
    EXPECT(c, "funA(x) = 2 * 4 + x",             "x + 8");
    EXPECT(c, "funB(x) = 4 -5 + (x + 2)^2 - 4",  "x^2 + 4 * x - 1");
    EXPECT(c, "funC(x) = 4x + 5 - 2",            "4 * x + 3");
    EXPECT(c, "funA(2) + funB(4) = ?",           "41");
    EXPECT(c, "funC(3) = ?",                     "15");
}

static void subjectPage11()
{
    section("Subject p10-11: syntax variants");
    Computor c;
    EXPECT(c, "funA(b) = 2*b+b",             "3 * b");
    EXPECT(c, "funB(a) =2 * a",              "2 * a");
    EXPECT(c, "funC(y) =2* y + 4 -2 * 4+1/3", "2 * y - 3.666666667");
    EXPECT(c, "funD(x)   =    2 *x",         "2 * x");
    EXPECT(c, "FUNA(5) = ?",                 "15");                          // names are case insensitive
}

// ---------------------------------------------------------------- simplification

static void simplification()
{
    section("Simplification");
    Computor c;
    EXPECT(c, "f(x) = (x^2 + 3x + 2) / (x + 1)", "x + 2");                   // exact polynomial division
    EXPECT(c, "f(x) = x - x",                    "0");
    EXPECT(c, "f(x) = (x + 1)^3",                "x^3 + 3 * x^2 + 3 * x + 1");
    EXPECT(c, "f(x) = (x + 1) * (x - 1)",        "x^2 - 1");
    EXPECT(c, "f(x) = (2x + 4) / 2",             "x + 2");                   // constant divisor
    EXPECT(c, "f(x) = 0 / (x + 1)",              "0");
    EXPECT(c, "f(x) = 5",                        "5");                       // constant function
    EXPECT(c, "f(x) = x^0",                      "1");
    EXPECT(c, "f(x) = (1 + i) * x",              "(1 + i) * x");              // complex coefficient
    EXPECT(c, "f(x) = i * x + 1",                "i * x + 1");
    EXPECT(c, "f(2) = ?",                        "1 + 2i");
    EXPECT(c, "f(x) = -x^2",                     "-x^2");
}

static void notSimplifiable()
{
    section("Kept as typed when it is not a polynomial");
    Computor c;
    EXPECT(c, "f(x) = 1 / x",           "1 / x");                            // remainder is not zero
    EXPECT(c, "f(4) = ?",               "0.25");
    EXPECT_ERROR(c, "f(0) = ?",         "division by zero");                 // reported when called
    EXPECT(c, "g(x) = (x^2 + 1) / (x + 1)", "(x^2 + 1) / (x + 1)");
    EXPECT(c, "g(1) = ?",               "1");
    EXPECT(c, "h(y) = 43 * y / (4 % 2 * y)", "43 * y / (4 % 2 * y)");
    EXPECT_ERROR(c, "h(3) = ?",         "division by zero");                 // denominator is 0 * y
    EXPECT(c, "k(x) = x ^ -1",          "x^(-1)");
    EXPECT_ERROR(c, "k(2) = ?",         "exponent");
    EXPECT(c, "m(x) = x % 3",           "x % 3");
    EXPECT(c, "m(7) = ?",               "1");
}

// ---------------------------------------------------------------- calls and composition

static void calls()
{
    section("Calls");
    Computor c;
    EXPECT(c, "f(x) = 2 * x + 1", "2 * x + 1");
    EXPECT(c, "f(3) = ?",         "7");
    EXPECT(c, "f(3) + f(4) = ?",  "16");
    EXPECT(c, "f(f(1)) = ?",      "7");
    EXPECT(c, "f(1 + 2i) = ?",    "3 + 4i");
    EXPECT(c, "a = f(10)",        "21");
    EXPECT(c, "a + 1 = ?",        "22");
    EXPECT(c, "f(a) = ?",         "43");
    EXPECT(c, "2 * f(1) ^ 2 = ?", "18");
    EXPECT(c, "f = ?",            "f(x) = 2 * x + 1");                       // show a function
}

static void composition()
{
    section("Composition (bonus)");
    Computor c;
    EXPECT(c, "funA(x) = 2*x+1",       "2 * x + 1");
    EXPECT(c, "funB(x) = 2 * x+1",     "2 * x + 1");
    EXPECT(c, "funA(funB(x)) = ?",     "4 * x + 3");
    EXPECT(c, "funA(funB(2)) = ?",     "11");
    EXPECT(c, "funC(t) = funA(t) * funB(t)", "4 * t^2 + 4 * t + 1");        // calls inside a definition
    EXPECT(c, "funC(1) = ?",           "9");
    EXPECT(c, "funA(x + 1) = ?",       "2 * x + 3");
    EXPECT(c, "funA(x) = ?",           "2 * x + 1");
}

static void snapshot()
{
    section("A function keeps what it was built from");
    Computor c;
    EXPECT(c, "a = 5",                  "5");
    EXPECT(c, "f(x) = x + a",           "x + 5");
    EXPECT(c, "a = 100",                "100");
    EXPECT(c, "f(1) = ?",               "6");                                // variables are read at definition time
    EXPECT(c, "g(x) = x ^ 2",           "x^2");
    EXPECT(c, "h(x) = g(x) + g(x + 1)", "2 * x^2 + 2 * x + 1");
    EXPECT(c, "g(x) = 0",               "0");
    EXPECT(c, "h(2) = ?",               "13");                               // h still uses the old g
    EXPECT(c, "f(x) = f(x) + 1",        "x + 6");                            // redefine from the previous f: no recursion
    EXPECT(c, "f(0) = ?",               "6");
}

static void matrices()
{
    section("Functions and matrices");
    Computor c;
    EXPECT(c, "m = [[1,2];[3,4]]",               "[ 1 , 2 ]\n  [ 3 , 4 ]");
    EXPECT(c, "f(x) = 2 * x",                    "2 * x");
    EXPECT(c, "f(m) = ?",                        "[ 2 , 4 ]\n  [ 6 , 8 ]");   // a polynomial can take a matrix
    EXPECT(c, "g(x) = x ** m",                   "x ** [[1,2];[3,4]]");
    EXPECT(c, "g([[1,0];[0,1]]) = ?",            "[ 1 , 2 ]\n  [ 3 , 4 ]");
    EXPECT_ERROR(c, "g(2) = ?",                  "matrix");
    EXPECT(c, "h(x) = m * x",                    "[[1,2];[3,4]] * x");
    EXPECT(c, "h(2) = ?",                        "[ 2 , 4 ]\n  [ 6 , 8 ]");
    EXPECT(c, "p(x) = x + m",                    "x + [[1,2];[3,4]]");       // only wrong once x is known
    EXPECT_ERROR(c, "p(2) = ?",                  "cannot use '+'");
}

// ---------------------------------------------------------------- errors

static void errors()
{
    section("Errors");
    Computor c;
    EXPECT(c, "f(x) = x + 1", "x + 1");
    EXPECT_ERROR(c, "g(x) = y + 1",         "unknown variable 'y'");
    EXPECT_ERROR(c, "g(x) = nope(x)",       "unknown function 'nope'");
    EXPECT_ERROR(c, "g(x) = 1 / 0",         "division by zero");             // fixed value: reported now
    EXPECT_ERROR(c, "undefinedfn(2) = ?",   "unknown function");
    EXPECT_ERROR(c, "g(2) = ?",             "unknown function 'g'");         // g was never defined
    EXPECT_ERROR(c, "f(1, 2) = ?",          "')'");
    EXPECT_ERROR(c, "f(x) + g(2) = ?",      "unknown function 'g'");
    EXPECT(c, "a = 3", "3");
    EXPECT_ERROR(c, "a(2) = ?",             "'a' is a variable");
    EXPECT_ERROR(c, "f = 3 + x",            "unknown variable 'x'");
    EXPECT(c, "f(x) = 5 * x",               "5 * x");                        // a failed line changes nothing
    EXPECT(c, "f(1) = ?",                   "5");
    EXPECT_ERROR(c, "f(x) + f(y) = ?",      "several unknowns");             // only one unknown is supported
    EXPECT_ERROR(c, "x = ?",                "unknown variable 'x'");         // a bare unknown is still an error
    EXPECT_ERROR(c, "f(x) = y ?",           "several unknowns");            // solving is Sprint 6
}

static void functionVsVariable()
{
    section("Names: variable <-> function");
    Computor c;
    EXPECT(c, "a = 2",       "2");
    EXPECT(c, "a(x) = x + 1", "x + 1");                                      // becomes a function
    EXPECT_ERROR(c, "a + 1 = ?", "'a' is a function");
    EXPECT(c, "a = 7",       "7");                                           // and back
    EXPECT(c, "a + 1 = ?",   "8");
    EXPECT_ERROR(c, "a(2) = ?", "'a' is a variable");
}

// ---------------------------------------------------------------- Simplifier directly

static void simplifierUnit()
{
    section("Simplifier (direct)");
    SymbolTable t;
    t.set("a", Value(Complex(3)));
    Simplifier  s(t);

    NodePtr e = Parser::parseExpression("a * x + 2");
    EXPECT_TRUE(s.freeVariables(e).size() == 1 && *s.freeVariables(e).begin() == "x");
    EXPECT_TRUE(s.substitute(e, "x")->toString() == "3 * x + 2");
    Polynomial p;
    EXPECT_TRUE(s.toPolynomial(s.substitute(e, "x"), "x", p));
    EXPECT_TRUE(p.toString("x") == "3 * x + 2");
    EXPECT_TRUE(!s.toPolynomial(Parser::parseExpression("1 / x"), "x", p));
    EXPECT_TRUE(!s.toPolynomial(Parser::parseExpression("[[1]] * x"), "x", p));
    EXPECT_TRUE(Simplifier::mentions(e, "x") && !Simplifier::mentions(e, "y"));
    EXPECT_TRUE(Simplifier::containsCall(Parser::parseExpression("2 + f(1)")));
    EXPECT_TRUE(!Simplifier::containsCall(Parser::parseExpression("2 + a")));
    NodePtr r = Simplifier::replace(Parser::parseExpression("x * x + 1"), "x", Parser::parseExpression("y + 1"));
    EXPECT_TRUE(r->toString() == "(y + 1) * (y + 1) + 1");
}

// ---------------------------------------------------------------- main

int main()
{
    subjectAssignment();
    subjectPage8();
    subjectPage9();
    subjectPage11();
    simplification();
    notSimplifiable();
    calls();
    composition();
    snapshot();
    matrices();
    errors();
    functionVsVariable();
    simplifierUnit();

    std::cout << "\n" << (g_fail ? "\033[31m" : "\033[32m")
              << g_pass << " passed, " << g_fail << " failed\033[0m" << std::endl;
    return g_fail ? 1 : 0;
}
