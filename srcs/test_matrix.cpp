// Unit tests for Matrix and for matrices through Value / Computor (S4).
// Build & run:  make unit

#include "../includes/Matrix.hpp"
#include "../includes/Computor.hpp"
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

// Rows are joined with " | " so one expected string covers a whole matrix
static std::string flat(const std::string &s)
{
    std::string out;
    for (size_t k = 0; k < s.size(); ++k)
    {
        if (s[k] == '\n')
            out += " | ";
        else
            out += s[k];
    }
    return out;
}

// Any Matrix expression, compared through toString()
#define EXPECT_M(expr, want) \
    do { std::string _g; try { _g = flat((expr).toString()); } \
         catch (const std::exception &e) { _g = std::string("error: ") + e.what(); } \
         report(_g == (want), #expr, _g, (want), __LINE__); } while (0)

#define EXPECT_THROW(expr, part) \
    do { std::string _g = "<no exception>"; bool _ok = false; \
         try { _g = flat((expr).toString()); } \
         catch (const std::exception &e) { _g = e.what(); _ok = _g.find(part) != std::string::npos; } \
         report(_ok, #expr " should throw", _g, std::string("...") + (part) + "...", __LINE__); } while (0)

#define EXPECT_TRUE(cond) \
    report((cond), #cond, (cond) ? "true" : "false", "true", __LINE__)

// One computor line; the 2-space indent of each row is removed
static std::string run(Computor &c, const std::string &line)
{
    try
    {
        std::string out = c.process(line), clean;
        for (size_t k = 0; k < out.size(); ++k)
        {
            if ((k == 0 || out[k - 1] == '\n') && out.compare(k, 2, "  ") == 0)
                k += 2;
            if (k < out.size())
                clean += out[k];
        }
        return flat(clean);
    }
    catch (const std::exception &e) { return std::string("error: ") + e.what(); }
}

#define EXPECT(c, line, want) \
    do { std::string _g = run(c, line); report(_g == (want), "\"" line "\"", _g, (want), __LINE__); } while (0)

#define EXPECT_ERROR(c, line, part) \
    do { std::string _g = run(c, line); \
         bool _ok = _g.compare(0, 7, "error: ") == 0 && _g.find(part) != std::string::npos; \
         report(_ok, "\"" line "\" should fail", _g, std::string("error: ...") + (part) + "...", __LINE__); } while (0)

static void section(const char *name) { std::cout << "\033[1m" << name << "\033[0m\n"; }

typedef Complex C;

static Matrix M2(double a, double b, double c, double d)
{
    std::vector<std::vector<C> > r(2);
    r[0].push_back(C(a)); r[0].push_back(C(b));
    r[1].push_back(C(c)); r[1].push_back(C(d));
    return Matrix(r);
}

static Matrix row(double a, double b)
{
    std::vector<std::vector<C> > r(1);
    r[0].push_back(C(a)); r[0].push_back(C(b));
    return Matrix(r);
}

static Matrix col(double a, double b)
{
    std::vector<std::vector<C> > r(2);
    r[0].push_back(C(a));
    r[1].push_back(C(b));
    return Matrix(r);
}

// ---------------------------------------------------------------- Matrix class

static void matrixBasics()
{
    section("Matrix: construction and display");
    Matrix m = M2(1, 2, 3, 4);
    EXPECT_TRUE(m.rows() == 2 && m.cols() == 2 && m.isSquare());
    EXPECT_TRUE(m.at(1, 0) == C(3));
    EXPECT_TRUE(m.sizeString() == "2x2");
    EXPECT_M(m,                         "[ 1 , 2 ] | [ 3 , 4 ]");
    EXPECT_M(row(3, 4),                 "[ 3 , 4 ]");
    EXPECT_M(col(1, 2),                 "[ 1 ] | [ 2 ]");
    EXPECT_M(Matrix(2, 3),              "[ 0 , 0 , 0 ] | [ 0 , 0 , 0 ]");
    EXPECT_M(Matrix::identity(3),       "[ 1 , 0 , 0 ] | [ 0 , 1 , 0 ] | [ 0 , 0 , 1 ]");
    EXPECT_M(M2(0.5, -1, 1.0 / 3, 0),   "[ 0.5 , -1 ] | [ 0.333333333 , 0 ]");
    EXPECT_TRUE(!row(1, 2).isSquare());
    EXPECT_TRUE(M2(1, 2, 3, 4) == M2(1, 2, 3, 4));
    EXPECT_TRUE(!(M2(1, 2, 3, 4) == M2(1, 2, 3, 5)));
    EXPECT_TRUE(!(row(1, 2) == col(1, 2)));

    std::vector<std::vector<C> > ragged(2);
    ragged[0].push_back(C(1)); ragged[0].push_back(C(2));
    ragged[1].push_back(C(3));
    bool threw = false;
    try { Matrix bad(ragged); } catch (const std::runtime_error &) { threw = true; }
    EXPECT_TRUE(threw);
    threw = false;
    try { Matrix bad(0, 2); } catch (const std::runtime_error &) { threw = true; }
    EXPECT_TRUE(threw);
}

static void matrixEntryWise()
{
    section("Matrix: + - * term-to-term, scaling");
    Matrix a = M2(1, 2, 3, 4), b = M2(5, 6, 7, 8);
    EXPECT_M(a + b,                     "[ 6 , 8 ] | [ 10 , 12 ]");
    EXPECT_M(a - b,                     "[ -4 , -4 ] | [ -4 , -4 ]");
    EXPECT_M(a * b,                     "[ 5 , 12 ] | [ 21 , 32 ]");
    EXPECT_M(-a,                        "[ -1 , -2 ] | [ -3 , -4 ]");
    EXPECT_M(a * C(2),                  "[ 2 , 4 ] | [ 6 , 8 ]");
    EXPECT_M(a / C(2),                  "[ 0.5 , 1 ] | [ 1.5 , 2 ]");
    EXPECT_M(a * C::i(),                "[ i , 2i ] | [ 3i , 4i ]");
    EXPECT_THROW(a + row(1, 2),         "different sizes (2x2 and 1x2)");
    EXPECT_THROW(a - row(1, 2),         "cannot subtract");
    EXPECT_THROW(a * row(1, 2),         "term-to-term");
    EXPECT_THROW(a / C(0),              "division by zero");
}

static void matrixProduct()
{
    section("Matrix: ** product");
    Matrix a = M2(1, 2, 3, 4), b = M2(5, 6, 7, 8);
    EXPECT_M(a.multiply(b),             "[ 19 , 22 ] | [ 43 , 50 ]");
    EXPECT_M(b.multiply(a),             "[ 23 , 34 ] | [ 31 , 46 ]");    // not commutative
    EXPECT_M(a.multiply(col(5, 6)),     "[ 17 ] | [ 39 ]");
    EXPECT_M(row(1, 2).multiply(col(3, 4)), "[ 11 ]");                   // 1x2 ** 2x1 = 1x1
    EXPECT_M(col(1, 2).multiply(row(3, 4)), "[ 3 , 4 ] | [ 6 , 8 ]");   // 2x1 ** 1x2 = 2x2
    EXPECT_M(a.multiply(Matrix::identity(2)), "[ 1 , 2 ] | [ 3 , 4 ]");
    EXPECT_THROW(row(1, 2).multiply(row(1, 2)), "cannot multiply 1x2 ** 1x2");
    EXPECT_THROW(a.multiply(row(1, 2)), "as many columns");
}

static void matrixPower()
{
    section("Matrix: ^");
    Matrix a = M2(1, 2, 3, 4);
    EXPECT_M(a.pow(C(0)),               "[ 1 , 0 ] | [ 0 , 1 ]");
    EXPECT_M(a.pow(C(1)),               "[ 1 , 2 ] | [ 3 , 4 ]");
    EXPECT_M(a.pow(C(2)),               "[ 7 , 10 ] | [ 15 , 22 ]");
    EXPECT_M(a.pow(C(3)),               "[ 37 , 54 ] | [ 81 , 118 ]");
    EXPECT_TRUE(a.pow(C(5)) == a.multiply(a).multiply(a).multiply(a).multiply(a));
    EXPECT_M(M2(0, 1, 1, 0).pow(C(100)), "[ 1 , 0 ] | [ 0 , 1 ]");      // swap matrix
    EXPECT_THROW(row(1, 2).pow(C(2)),   "square");
    EXPECT_THROW(a.pow(C(-1)),          "integer >= 0");
    EXPECT_THROW(a.pow(C(1.5)),         "integer >= 0");
    EXPECT_THROW(a.pow(C::i()),         "integer >= 0");
}

static void matrixComplex()
{
    section("Matrix: complex entries");
    std::vector<std::vector<C> > r(1);
    r[0].push_back(C::i()); r[0].push_back(C(1, 1));
    Matrix z(r);
    EXPECT_M(z,                         "[ i , 1 + i ]");
    EXPECT_M(z * z,                     "[ -1 , 2i ]");
    EXPECT_M(z * C::i(),                "[ -1 , -1 + i ]");
    std::vector<std::vector<C> > c(2);
    c[0].push_back(C(1));
    c[1].push_back(C::i());
    EXPECT_M(z.multiply(Matrix(c)),     "[ -1 + 2i ]");
}

// ---------------------------------------------------------------- through the computor

static void subjectExamples()
{
    section("Subject: matrix assignments (p.7, p.10)");
    Computor c;
    EXPECT(c, "varA = [[2,3];[4,3]]",           "[ 2 , 3 ] | [ 4 , 3 ]");
    EXPECT(c, "varB = [[3,4]]",                 "[ 3 , 4 ]");
    EXPECT(c, "matA = [[1,2];[3,2];[3,4]]",     "[ 1 , 2 ] | [ 3 , 2 ] | [ 3 , 4 ]");
    EXPECT(c, "matB=   [[1,2]]",                "[ 1 , 2 ]");
    EXPECT(c, "c = [[1];[2];[3]]",              "[ 1 ] | [ 2 ] | [ 3 ]");
}

static void computorMatrices()
{
    section("Computor: matrix variables and operators");
    Computor c;
    EXPECT(c, "m = [[1,2];[3,4]]",              "[ 1 , 2 ] | [ 3 , 4 ]");
    EXPECT(c, "n = [[5,6];[7,8]]",              "[ 5 , 6 ] | [ 7 , 8 ]");
    EXPECT(c, "m + n = ?",                      "[ 6 , 8 ] | [ 10 , 12 ]");
    EXPECT(c, "m * n = ?",                      "[ 5 , 12 ] | [ 21 , 32 ]");
    EXPECT(c, "m ** n = ?",                     "[ 19 , 22 ] | [ 43 , 50 ]");
    EXPECT(c, "2 * m = ?",                      "[ 2 , 4 ] | [ 6 , 8 ]");
    EXPECT(c, "m * 2 = ?",                      "[ 2 , 4 ] | [ 6 , 8 ]");
    EXPECT(c, "m / 2 = ?",                      "[ 0.5 , 1 ] | [ 1.5 , 2 ]");
    EXPECT(c, "-m = ?",                         "[ -1 , -2 ] | [ -3 , -4 ]");
    EXPECT(c, "m ^ 2 = ?",                      "[ 7 , 10 ] | [ 15 , 22 ]");
    EXPECT(c, "m ^ 0 = ?",                      "[ 1 , 0 ] | [ 0 , 1 ]");
    EXPECT(c, "m ** [[5];[6]] = ?",             "[ 17 ] | [ 39 ]");
    EXPECT(c, "2 * m ** n = ?",                 "[ 38 , 44 ] | [ 86 , 100 ]");   // same level, left to right
    EXPECT(c, "m + n * 2 = ?",                  "[ 11 , 14 ] | [ 17 , 20 ]");    // * before +
    EXPECT(c, "(m + n) * 2 = ?",                "[ 12 , 16 ] | [ 20 , 24 ]");
    EXPECT(c, "m = ?",                          "[ 1 , 2 ] | [ 3 , 4 ]");        // unchanged
}

static void entriesAndTypes()
{
    section("Computor: entries are expressions, types change");
    Computor c;
    EXPECT(c, "v = 2",                          "2");
    EXPECT(c, "w = [[v, 2 * 3];[v ^ 2, 1 / 4]]", "[ 2 , 6 ] | [ 4 , 0.25 ]");
    EXPECT(c, "v = 10",                         "10");
    EXPECT(c, "w = ?",                          "[ 2 , 6 ] | [ 4 , 0.25 ]");     // entries were copied
    EXPECT(c, "z = [[i, 1 + i]]",               "[ i , 1 + i ]");
    EXPECT(c, "z * z = ?",                      "[ -1 , 2i ]");
    EXPECT(c, "z ** [[1];[i]] = ?",             "[ -1 + 2i ]");
    EXPECT(c, "x = 5",                          "5");
    EXPECT(c, "x = [[1,2]]",                    "[ 1 , 2 ]");                    // number -> matrix
    EXPECT_TRUE(c.variables().get("x").type() == Value::MATRIX);
    EXPECT(c, "x = 3i",                         "3i");                           // matrix -> complex
    EXPECT_TRUE(c.variables().get("x").type() == Value::COMPLEX);
}

static void computorErrors()
{
    section("Computor: matrix errors");
    Computor c;
    EXPECT(c, "m = [[1,2];[3,4]]",              "[ 1 , 2 ] | [ 3 , 4 ]");
    EXPECT(c, "r = [[1,2]]",                    "[ 1 , 2 ]");
    EXPECT_ERROR(c, "m + r = ?",                "different sizes (2x2 and 1x2)");
    EXPECT_ERROR(c, "r ** r = ?",               "cannot multiply 1x2 ** 1x2");
    EXPECT_ERROR(c, "r ^ 2 = ?",                "square");
    EXPECT_ERROR(c, "m + 1 = ?",                "cannot use '+' between a matrix and a rational");
    EXPECT_ERROR(c, "1 - m = ?",                "cannot use '-' between a rational and a matrix");
    EXPECT_ERROR(c, "m % 2 = ?",                "cannot use '%'");
    EXPECT_ERROR(c, "m / m = ?",                "matrix division is not defined");
    EXPECT_ERROR(c, "2 / m = ?",                "cannot use '/' between a rational and a matrix");
    EXPECT_ERROR(c, "2 ** m = ?",               "use '*' to scale");
    EXPECT_ERROR(c, "m ** 2 = ?",               "use '*' to scale");
    EXPECT_ERROR(c, "2 ** 2 = ?",               "'**' is matrix multiplication");
    EXPECT_ERROR(c, "m ^ m = ?",                "cannot use '^' between a matrix and a matrix");
    EXPECT_ERROR(c, "2 ^ m = ?",                "cannot use '^' between a rational and a matrix");
    EXPECT_ERROR(c, "m ^ -1 = ?",               "integer >= 0");
    EXPECT_ERROR(c, "m ^ i = ?",                "integer >= 0");
    EXPECT_ERROR(c, "m / 0 = ?",                "division by zero");
    EXPECT_ERROR(c, "[[ [[1]] ]] = ?",          "must be a number");
    EXPECT_ERROR(c, "[[1,2];[3]] = ?",          "same length");
    EXPECT_ERROR(c, "m = m + r",                "different sizes");
    EXPECT(c, "m = ?",                          "[ 1 , 2 ] | [ 3 , 4 ]");        // failed line kept m
}

// ---------------------------------------------------------------- main

int main()
{
    matrixBasics();
    matrixEntryWise();
    matrixProduct();
    matrixPower();
    matrixComplex();
    subjectExamples();
    computorMatrices();
    entriesAndTypes();
    computorErrors();

    std::cout << "\n" << (g_fail ? "\033[31m" : "\033[32m")
              << g_pass << " passed, " << g_fail << " failed\033[0m" << std::endl;
    return g_fail ? 1 : 0;
}