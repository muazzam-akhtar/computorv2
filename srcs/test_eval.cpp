// Unit tests for Value, SymbolTable, Evaluator and Computor (S3: variables).
// Build & run:  make unit

#include "../includes/Computor.hpp"
#include "../includes/Evaluator.hpp"
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

// Feeds one line to the computor; errors come back as "error: <message>"
static std::string run(Computor &c, const std::string &line)
{
    try { return c.process(line); }
    catch (const std::exception &e) { return std::string("error: ") + e.what(); }
}

// Output of a line, without the 2-space indent
#define EXPECT(c, line, want) \
    do { std::string _g = run(c, line); if (_g.compare(0, 2, "  ") == 0) _g = _g.substr(2); \
         report(_g == (want), "\"" line "\"", _g, (want), __LINE__); } while (0)

// The line must fail and the message must contain `part`
#define EXPECT_ERROR(c, line, part) \
    do { std::string _g = run(c, line); \
         bool _ok = _g.compare(0, 7, "error: ") == 0 && _g.find(part) != std::string::npos; \
         report(_ok, "\"" line "\" should fail", _g, std::string("error: ...") + (part) + "...", __LINE__); } while (0)

#define EXPECT_TRUE(cond) \
    report((cond), #cond, (cond) ? "true" : "false", "true", __LINE__)

static void section(const char *name) { std::cout << "\033[1m" << name << "\033[0m\n"; }

// ---------------------------------------------------------------- Value

static void valueTypes()
{
    section("Value: type inference");
    EXPECT_TRUE(Value(Complex(2)).type() == Value::RATIONAL);
    EXPECT_TRUE(Value(Complex(2)).typeName() == "rational");
    EXPECT_TRUE(Value(Complex(3, 2)).type() == Value::COMPLEX);
    EXPECT_TRUE(Value(Complex(3, 2)).typeName() == "complex");
    EXPECT_TRUE(Value(Complex::i() * Complex::i()).type() == Value::RATIONAL);   // i^2 = -1 is rational
    EXPECT_TRUE(Value().toString() == "0");
}

static void valueApply()
{
    section("Value: operator dispatch");
    Value two(Complex(2)), three(Complex(3)), z(Complex(1, 1));
    EXPECT_TRUE(Value::apply("+", two, three).toString() == "5");
    EXPECT_TRUE(Value::apply("-", two, three).toString() == "-1");
    EXPECT_TRUE(Value::apply("*", two, z).toString() == "2 + 2i");
    EXPECT_TRUE(Value::apply("/", three, two).toString() == "1.5");
    EXPECT_TRUE(Value::apply("%", three, two).toString() == "1");
    EXPECT_TRUE(Value::apply("^", z, two).toString() == "2i");
    EXPECT_TRUE(two.negate().toString() == "-2");
    bool threw = false;
    try { Value::apply("**", two, three); } catch (const std::runtime_error &) { threw = true; }
    EXPECT_TRUE(threw);
}

// ---------------------------------------------------------------- SymbolTable

static void symbolTable()
{
    section("SymbolTable");
    SymbolTable t;
    EXPECT_TRUE(!t.has("x"));
    t.set("x", Value(Complex(2)));
    EXPECT_TRUE(t.has("x"));
    EXPECT_TRUE(t.get("x").toString() == "2");
    t.set("x", Value(Complex(0, 1)));                          // replace with another type
    EXPECT_TRUE(t.get("x").toString() == "i");
    EXPECT_TRUE(t.all().size() == 1);
    t.remove("x");
    EXPECT_TRUE(!t.has("x"));
    bool threw = false;
    try { t.get("x"); } catch (const std::runtime_error &) { threw = true; }
    EXPECT_TRUE(threw);
}

// ---------------------------------------------------------------- Computor: subject

static void subjectPage7()
{
    section("Subject p.7: assignment of each type");
    Computor c;
    EXPECT(c, "varA = 2",               "2");
    EXPECT(c, "varB = 4.242",           "4.242");
    EXPECT(c, "varC = -4.3",            "-4.3");
    EXPECT(c, "varA = 2*i + 3",         "3 + 2i");
    EXPECT(c, "varB = -4i - 4",         "-4 - 4i");
}

static void subjectPage8()
{
    section("Subject p.8: reassignment and type change");
    Computor c;
    EXPECT(c, "x = 2",                  "2");
    EXPECT(c, "y = x",                  "2");
    EXPECT(c, "y = 7",                  "7");
    EXPECT(c, "y = 2 * i - 4",          "-4 + 2i");
    EXPECT(c, "varA = 2 + 4 *2 - 5 %4 + 2 * (4 + 5)", "27");
    EXPECT(c, "varB = 2 * varA - 5 %4", "53");
    EXPECT(c, "varC = 2 * varA - varB", "1");
}

static void subjectPage9()
{
    section("Subject p.9: computation with = ?");
    Computor c;
    EXPECT(c, "a = 2 * 4 + 4",          "12");
    EXPECT(c, "a + 2 = ?",              "14");
}

static void subjectPage10()
{
    section("Subject p.10: syntax variations");
    Computor c;
    EXPECT(c, "varA = 2",               "2");
    EXPECT(c, "varB= 2 * (4 + varA + 3)", "18");
    EXPECT(c, "varC =2 * varB",         "36");
    EXPECT(c, "varD    =    2 *(2 + 4 *varC -4 /3)", "289.333333333");
    EXPECT(c, "xx = 3",                 "3");
    EXPECT(c, "2 * xx = ?",             "6");          // xx is a variable, not x * x
}

// ---------------------------------------------------------------- Computor: rules

static void variableRules()
{
    section("Variables: names, copies, queries");
    Computor c;
    EXPECT(c, "varA = 2",               "2");
    EXPECT(c, "VARA = ?",               "2");          // case-insensitive
    EXPECT(c, "vara = ?",               "2");
    EXPECT(c, "VaRa = 3",               "3");          // same variable
    EXPECT(c, "varA = ?",               "3");
    EXPECT_TRUE(c.variables().all().size() == 1);

    EXPECT(c, "x = 2",                  "2");
    EXPECT(c, "y = x",                  "2");
    EXPECT(c, "x = 5",                  "5");
    EXPECT(c, "y = ?",                  "2");          // y kept a copy, not a link

    EXPECT(c, "x = x * 2 + 1",          "11");         // self-reference uses the old value
    EXPECT(c, "x = 2 + 3i",             "2 + 3i");     // rational -> complex
    EXPECT(c, "x * x = ?",              "-5 + 12i");
    EXPECT(c, "x = x - 3i",             "2");          // complex -> rational
    EXPECT_TRUE(c.variables().get("x").type() == Value::RATIONAL);
    EXPECT(c, "2 + 2",                  "4");          // bare expression
}

static void errorsKeepState()
{
    section("Errors: a failed line changes nothing");
    Computor c;
    EXPECT(c, "x = 5",                  "5");
    EXPECT_ERROR(c, "x = 1 / 0",        "division by zero");
    EXPECT(c, "x = ?",                  "5");
    EXPECT_ERROR(c, "x = undefinedvar", "unknown variable 'undefinedvar'");
    EXPECT(c, "x = ?",                  "5");
    EXPECT_ERROR(c, "x = 2 +",          "syntax error");
    EXPECT(c, "x = ?",                  "5");
    EXPECT_ERROR(c, "z = undefinedvar", "unknown variable");
    EXPECT_TRUE(!c.variables().has("z"));
    EXPECT_ERROR(c, "z = ?",            "unknown variable 'z'");
}

static void errorRules()
{
    section("Errors: operators and names");
    Computor c;
    EXPECT_ERROR(c, "i = 5",            "'i' is reserved");
    EXPECT_ERROR(c, "var1 = 5",         "invalid name");
    EXPECT_ERROR(c, "1 / 0 = ?",        "division by zero");
    EXPECT_ERROR(c, "5 % 0 = ?",        "by zero");
    EXPECT_ERROR(c, "i % 2 = ?",        "only defined for real numbers");
    EXPECT_ERROR(c, "2 ** 2 = ?",       "matrix multiplication");
    EXPECT_ERROR(c, "2 ^ -1 = ?",       "exponent");
    EXPECT_ERROR(c, "2 ^ 0.5 = ?",      "exponent");
    EXPECT_ERROR(c, "2 ^ i = ?",        "exponent");
    EXPECT(c, "5.5 % 2 = ?",            "1.5");        // decimals allowed
    EXPECT(c, "-5 % 4 = ?",             "-1");         // sign of the left operand
    EXPECT(c, "0 ^ 0 = ?",              "1");
}

static void indentation()
{
    section("Output: two-space indent");
    Computor c;
    EXPECT_TRUE(c.process("x = 2") == "  2");
    EXPECT_TRUE(c.process("x + 1 = ?") == "  3");
}

// ---------------------------------------------------------------- main

int main()
{
    valueTypes();
    valueApply();
    symbolTable();
    subjectPage7();
    subjectPage8();
    subjectPage9();
    subjectPage10();
    variableRules();
    errorsKeepState();
    errorRules();
    indentation();

    std::cout << "\n" << (g_fail ? "\033[31m" : "\033[32m")
              << g_pass << " passed, " << g_fail << " failed\033[0m" << std::endl;
    return g_fail ? 1 : 0;
}