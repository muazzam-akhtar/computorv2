// test_all.cpp  — merged Rational + Complex + Matrix + Polynomial tests
#include <iostream>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>
#include "../includes/Rational.hpp"
#include "../includes/Complex.hpp"
#include "../includes/Matrix.hpp"
#include "../includes/Polynomial.hpp"

static int g_pass = 0, g_fail = 0;

void check(bool cond, const std::string &name, const std::string &got = "")
{
    if (cond) { ++g_pass; std::cout << "[PASS] " << name << "\n"; }
    else      { ++g_fail; std::cout << "[FAIL] " << name
                                    << (got.empty() ? "" : ("  got: " + got)) << "\n"; }
}

void checkThrows(std::function<void()> fn, const std::string &name)
{
    try { fn(); check(false, name + " (expected throw)"); }
    catch (const std::exception &) { check(true, name); }
}

using R  = Rational;
using C  = Complex;
using RM = Matrix<R>;
using CM = Matrix<C>;
using RP = std::vector<R>;   // polynomial as coefficient vector
using CP = std::vector<C>;

static R r(long long n, long long d = 1) { return R(n, d); }
static C c(long long re, long long im = 0) { return C(r(re), r(im)); }

int main()
{
    /* ============================================================
     *  Rational
     * ============================================================ */
    std::cout << "=== Rational tests ===\n";
    // ... (unchanged — see full file body below) ...
    check(R(2, 4).toString()  == "1/2",  "2/4 -> 1/2",  R(2,4).toString());
    check(R(4, -6).toString() == "-2/3", "4/-6 -> -2/3", R(4,-6).toString());
    check(R(0, 5).toString()  == "0",    "0/5 -> 0",    R(0,5).toString());
    check(R(6, 3).toString()  == "2",    "6/3 -> 2",    R(6,3).toString());
    check(R().toString()      == "0",    "default -> 0", R().toString());
    check(r(7).toString()     == "7",    "7/1 -> 7",     r(7).toString());

    R a(1,2), b(1,3);
    check((a+b).toString() == "5/6", "1/2 + 1/3", (a+b).toString());
    check((a-b).toString() == "1/6", "1/2 - 1/3", (a-b).toString());
    check((a*b).toString() == "1/6", "1/2 * 1/3", (a*b).toString());
    check((a/b).toString() == "3/2", "1/2 / 1/3", (a/b).toString());
    check((R(7,2) % R(3,2)).toString() == "1/2", "7/2 % 3/2");
    check(R(2,3).pow(3).toString()  == "8/27", "2/3 ^ 3");
    check(R(2,3).pow(0).toString()  == "1",    "2/3 ^ 0");
    check(R(2,3).pow(-2).toString() == "9/4",  "2/3 ^ -2");
    check(R(0,1).isZero(),  "0 isZero");
    check(!R(1,1).isZero(), "1 not isZero");
    checkThrows([](){ R(1,0); },          "denominator 0 throws");
    checkThrows([](){ R(1,2) / R(0,1); }, "divide by 0 throws");
    checkThrows([](){ R(1,2) % R(0,1); }, "modulo by 0 throws");
    checkThrows([](){ R(0,1).pow(-1); },  "0 ^ -1 throws");

    /* ============================================================
     *  Complex
     * ============================================================ */
    std::cout << "\n=== Complex tests ===\n";
    C c1(r(1,2), r(1,3));
    C c2(r(1,2), r(-1,3));
    check(c1.toString() == "1/2 + 1/3i", "complex toString (+)");
    check(c2.toString() == "1/2 - 1/3i", "complex toString (-)");
    check(C(r(3), r(0)).toString()    == "3",        "complex real-only toString");
    check(C(r(0), r(5,2)).toString()  == "0 + 5/2i", "complex imag-only toString");
    check(C(r(0), r(-5,2)).toString() == "0 - 5/2i", "complex neg imag toString");
    check((c1 + r(1,2)).toString() == "1 + 1/3i",   "c + 1/2");
    check((c1 - r(1,2)).toString() == "0 + 1/3i",   "c - 1/2");
    check((c1 * r(2)).toString()   == "1 + 2/3i",   "c * 2");
    check((c1 / r(2)).toString()   == "1/4 + 1/6i", "c / 2");
    check(C(r(0), r(0)).isZero(), "0+0i isZero");
    check(!c1.isZero(),           "c1 not isZero");
    C ccopy(c1);
    ccopy = C(r(9), r(9));
    check(c1.toString() == "1/2 + 1/3i", "complex copy assignment independent");
    checkThrows([&](){ c1.pow(-1); }, "complex pow(-1) throws");

    /* ============================================================
     *  Matrix<Rational>
     * ============================================================ */
    std::cout << "\n=== Matrix<Rational> ===\n";
    RM ma({ {r(1), r(2)}, {r(3), r(4)} });
    check(ma.toString() == "[[1, 2], [3, 4]]", "2x2 toString");
    RM row({ {r(1), r(2), r(3)} });
    check(row.toString() == "[[1, 2, 3]]", "1x3 row toString");
    RM single({ { r(7) } });
    check(single.toString() == "[[7]]", "1x1 toString");
    RM macopy(ma);
    macopy = RM({ { r(9), r(9) }, { r(9), r(9) } });
    check(ma.toString() == "[[1, 2], [3, 4]]", "matrix copy assign independent");
    RM mb({ {r(10), r(20)}, {r(30), r(40)} });
    check((ma + mb).toString() == "[[11, 22], [33, 44]]", "matrix a + b");
    RM mh ({ {r(1,2), r(1,3)} });
    RM mh2({ {r(1,2), r(2,3)} });
    check((mh + mh2).toString() == "[[1, 1]]", "matrix fraction add");
    check((mb - ma).toString() == "[[9, 18], [27, 36]]", "matrix b - a");
    check((ma - ma).toString() == "[[0, 0], [0, 0]]", "matrix a - a = zero");
    RM id({ {r(1), r(0)}, {r(0), r(1)} });
    check((ma * id).toString() == "[[1, 2], [3, 4]]", "matrix a * I = a");
    RM me({ {r(5), r(6)}, {r(7), r(8)} });
    check((ma * me).toString() == "[[19, 22], [43, 50]]", "matrix a * e");
    RM mlhs({ {r(1), r(2), r(3)}, {r(4), r(5), r(6)} });
    RM mrhs({ {r(7), r(8)}, {r(9), r(10)}, {r(11), r(12)} });
    check((mlhs * mrhs).toString() == "[[58, 64], [139, 154]]", "matrix (2x3)*(3x2)");
    RM mfr ({ {r(1,2)} });
    RM mfr2({ {r(2,3)} });
    check((mfr * mfr2).toString() == "[[1/3]]", "matrix 1/2 * 2/3");
    RM threeCol({ {r(1), r(2), r(3)} });
    RM twoRow({ {r(1), r(2)}, {r(3), r(4)} });
    checkThrows([&]{ ma + threeCol; }, "matrix + row-size mismatch");
    checkThrows([&]{ twoRow + row;  }, "matrix + row-count mismatch");
    checkThrows([&]{ ma - threeCol; }, "matrix - row-size mismatch");
    checkThrows([&]{ mlhs * ma;     }, "matrix * inner-dim mismatch");
    RM empty({});
    checkThrows([&]{ ma * empty; }, "matrix * empty rhs");
    checkThrows([&]{ empty * ma; }, "matrix * empty lhs");

    /* ============================================================
     *  Matrix<Complex>
     * ============================================================ */
    std::cout << "\n=== Matrix<Complex> ===\n";
    CM ca({ { C(r(1), r(1)), C(r(2), r(0)) } });
    check(ca.toString() == "[[1 + 1i, 2]]", "complex matrix 1x2 toString");
    CM cb({ { C(r(1), r(-1)), C(r(2), r(1)) } });
    check((ca + cb).toString() == "[[2, 4 + 1i]]", "complex matrix add");
    check((ca - cb).toString() == "[[0 + 2i, 0 - 1i]]", "complex matrix sub");
    CM ci({ { C(r(1), r(1)) } });
    check((ci * ci).toString() == "[[0 + 2i]]", "complex matrix (1+i)^2 = 2i");
    CM cm({ { C(r(1), r(1)),  C(r(0), r(0)) },
            { C(r(0), r(0)),  C(r(1), r(-1)) } });
    check((cm * cm).toString() == "[[0 + 2i, 0], [0, 0 - 2i]]",
          "complex diag(1+i, 1-i) squared");
    CM c3({ { C(r(1), r(0)), C(r(2), r(0)), C(r(3), r(0)) } });
    checkThrows([&]{ ca + c3; }, "complex matrix + row-size mismatch");

    /* ============================================================
     *  Polynomial<Rational>
     * ============================================================ */
    std::cout << "\n=== Polynomial<Rational> ===\n";

    // p = 1 + 2x + 3x^2   ->  [1, 2, 3]
    RP p = { r(1), r(2), r(3) };
    // q = 2 - x            ->  [2, -1]
    RP q = { r(2), r(-1) };

    // toString
    check(poly_toString(p) == "3x^2 + 2x + 1",
          "poly toString quadratic", poly_toString(p));
    check(poly_toString(q) == "-x + 2",
          "poly toString linear", poly_toString(q));
    check(poly_toString(RP{}) == "0", "poly toString empty -> 0");
    check(poly_toString(RP{ r(0) }) == "0", "poly toString all-zero -> 0");
    check(poly_toString(RP{ r(5) }) == "5", "poly toString constant");
    check(poly_toString(RP{ r(-5) }) == "-5", "poly toString neg constant");
    check(poly_toString(p, "y") == "3y^2 + 2y + 1",
          "poly toString custom var", poly_toString(p, "y"));

    // add: (1+2x+3x^2) + (2-x) = 3 + x + 3x^2  ->  [3,1,3]
    check(poly_toString(poly_add(p, q)) == "3x^2 + x + 3",
          "poly add", poly_toString(poly_add(p, q)));
    // sub: (1+2x+3x^2) - (2-x) = -1 + 3x + 3x^2
    check(poly_toString(poly_sub(p, q)) == "3x^2 + 3x - 1",
          "poly sub", poly_toString(poly_sub(p, q)));
    // p - p = 0
    check(poly_toString(poly_sub(p, p)) == "0", "poly p - p = 0");

    // mul: (1+2x+3x^2)(2-x) = 2 + 3x + 4x^2 - 3x^3
    check(poly_toString(poly_mul(p, q)) == "-3x^3 + 4x^2 + 3x + 2",
          "poly mul", poly_toString(poly_mul(p, q)));

    // scale: 2 * p = 2 + 4x + 6x^2
    check(poly_toString(poly_scale(p, r(2))) == "6x^2 + 4x + 2",
          "poly scale by 2", poly_toString(poly_scale(p, r(2))));

    // pow: (1+x)^2 = 1 + 2x + x^2
    check(poly_toString(poly_pow(RP{ r(1), r(1) }, 2)) == "x^2 + 2x + 1",
          "poly (1+x)^2", poly_toString(poly_pow(RP{ r(1), r(1) }, 2)));
    check(poly_toString(poly_pow(p, 0)) == "1", "poly p^0 = 1");
    checkThrows([](){ poly_pow(RP{ r(1) }, -1); }, "poly_pow(-1) throws");

    // eval: p(2) = 1 + 4 + 12 = 17
    check(poly_eval(p, r(2)).toString() == "17", "poly eval p(2) = 17");
    // p(0) = 1
    check(poly_eval(p, r(0)).toString() == "1",  "poly eval p(0) = 1");

    // derivative: d/dx (1+2x+3x^2) = 2 + 6x
    check(poly_toString(poly_derivative(p)) == "6x + 2",
          "poly derivative", poly_toString(poly_derivative(p)));
    check(poly_toString(poly_derivative(RP{ r(5) })) == "0",
          "poly derivative of constant = 0");
    check(poly_toString(poly_derivative(RP{})) == "0",
          "poly derivative of empty = 0");

    // integral of [1, 2, 3] with C=0 -> [0, 1, 1, 1]  (x + x^2 + x^3)
    // NOTE: poly_integral only instantiated for Rational/Complex but uses
    // K(Rational(i+1,1)) which does NOT construct for Complex. We'll test
    // only with Rational here.
    check(poly_toString(poly_integral(p, r(0))) == "x^3 + x^2 + x",
          "poly integral", poly_toString(poly_integral(p, r(0))));
    // integral of [1] with C=5 -> [5, 1]  (5 + x)
    check(poly_toString(poly_integral(RP{ r(1) }, r(5))) == "x + 5",
          "poly integral of constant with C=5");

    // divmod: (x^2 - 1) / (x - 1) = (x+1, 0)
    RP num   = { r(-1), r(0), r(1) };  // x^2 - 1
    RP den   = { r(-1), r(1) };        // x - 1
    auto qr  = poly_divmod(num, den);
    check(poly_toString(qr.first)  == "x + 1", "poly divmod quotient",
          poly_toString(qr.first));
    check(poly_toString(qr.second) == "0",     "poly divmod remainder",
          poly_toString(qr.second));
    checkThrows([](){ poly_divmod(RP{ r(1) }, RP{}); },
                "poly_divmod by empty throws");

    // ---- poly_roots (Rational) ----
    // x - 3 = 0 -> x = 3 (real)
    // NOTE: poly_roots(degree 1) returns -c[0]/c[1].
    auto roots1 = poly_roots(RP{ r(-3), r(1) });
    check(roots1.size() == 1, "poly_roots degree 1 count");
    check(roots1[0].toString() == "3", "poly_roots x - 3 -> 3",
          roots1[0].toString());

    // deg 2 and deg >2 currently throw (complex discriminant / not impl.)
    checkThrows([](){ poly_roots(RP{ r(1), r(0), r(1) }); },
                "poly_roots deg 2 throws (stub)");
    checkThrows([](){ poly_roots(RP{ r(0), r(0), r(0), r(1) }); },
                "poly_roots deg 3 throws (not impl.)");

    /* ============================================================
     *  Polynomial<Complex>
     * ============================================================ */
    std::cout << "\n=== Polynomial<Complex> ===\n";

    CP cp = { c(1,1), c(2,0) };       // (1+i) + 2x
    CP cq = { c(0,1), c(1,0) };       // i + x
    check(poly_toString(cp) == "2x + 1 + 1i",
          "complex poly toString", poly_toString(cp));
    check(poly_toString(poly_add(cp, cq)) == "3x + 1 + 2i",
          "complex poly add", poly_toString(poly_add(cp, cq)));
    // (1+i + 2x) * (i + x) = ...
    //   1+i times i        = -1 + i
    //   2x times i         = 2i x
    //   1+i times x        = (1+i) x
    //   2x times x         = 2x^2
    //   => 2x^2 + (1 + 3i) x + (-1 + i)
    check(poly_toString(poly_mul(cp, cq)) == "2x^2 + 1 + 3ix - 1 + 1i",
          "complex poly mul", poly_toString(poly_mul(cp, cq)));
    // eval cp at x = i: (1+i) + 2i = 1 + 3i
    check(poly_eval(cp, c(0,1)).toString() == "1 + 3i",
          "complex poly eval at i", poly_eval(cp, c(0,1)).toString());

    // poly_roots (Complex) — degree 1
    auto croots = poly_roots(CP{ c(-1,0), c(1,0) });  // x - 1 = 0
    check(croots.size() == 1, "complex poly_roots degree 1 count");
    check(croots[0].toString() == "1", "complex poly_roots x - 1 -> 1",
          croots[0].toString());

    /* ============================================================
     *  Summary
     * ============================================================ */
    std::cout << "\n=== Summary: " << g_pass << " passed, " << g_fail << " failed ===\n";
    return g_fail == 0 ? 0 : 1;
}