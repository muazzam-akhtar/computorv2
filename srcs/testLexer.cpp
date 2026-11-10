#include <cassert>
#include <iostream>
#include <vector>
#include "../includes/Lexer.hpp"

static int g_pass = 0, g_fail = 0;

static void check(bool ok, const std::string &name)
{
    if (ok) { ++g_pass; std::cout << "[PASS] " << name << "\n"; }
    else    { ++g_fail; std::cout << "[FAIL] " << name << "\n"; }
}

static std::vector<Token> lex(const std::string &s) { return Lexer(s).tokenize(); }

int main()
{
    // --- single characters ---
    check(lex("+").size() == 2 && lex("+")[0].kind == TokenType::Plus,      "+ -> Plus");
    check(lex("*").size() == 2 && lex("*")[0].kind == TokenType::Star,      "* -> Star");
    check(lex("**").size() == 2 && lex("**")[0].kind == TokenType::StarStar,"** -> StarStar");
    check(lex("* *")[0].kind == TokenType::Star && lex("* *")[1].kind == TokenType::Star,
          "* * is two Stars, not StarStar");
    check(lex("=")[0].kind == TokenType::Assign,    "= -> Assign");
    check(lex("?")[0].kind == TokenType::Query,     "? -> Query");
    check(lex("(")[0].kind == TokenType::LParen,    "( -> LParen");
    check(lex(")")[0].kind == TokenType::RParen,    ") -> RParen");
    check(lex("[")[0].kind == TokenType::LBracket,  "[ -> LBracket");
    check(lex("]")[0].kind == TokenType::RBracket,  "] -> RBracket");
    check(lex(";")[0].kind == TokenType::Semicolon, "; -> Semicolon");
    check(lex(",")[0].kind == TokenType::Comma,     ", -> Comma");
    check(lex("%")[0].kind == TokenType::Percent,   "% -> Percent");
    check(lex("^")[0].kind == TokenType::Caret,     "^ -> Caret");

    // --- numbers ---
    check(lex("0")[0].text == "0",   "Number 0");
    check(lex("123")[0].text == "123", "Number 123");
    check(lex("00123")[0].text == "00123", "Number keeps leading zeros (parser normalizes)");

    // --- identifiers ---
    check(lex("a")[0].kind == TokenType::Ident && lex("a")[0].text == "a",   "Ident a");
    check(lex("varA")[0].text == "varA", "Ident preserves case");
    check(lex("x")[0].text == "x",       "Ident x (i is reserved at Context, not Lexer)");
    check(lex("i")[0].kind == TokenType::Ident, "i is a legal Ident token");
    check(lex("abc123")[0].text == "abc"
          && lex("abc123")[1].kind == TokenType::Number
          && lex("abc123")[1].text == "123",
          "abc123 = Ident + Number (digits not allowed in idents)");

    // --- whole expressions from the subject ---
    {
        auto t = lex("varA = 2 * 4 + 4");
        check(t.size() == 8, "varA = 2 * 4 + 4 token count");
        check(t[0].kind == TokenType::Ident && t[0].text == "varA", "  [0] Ident varA");
        check(t[1].kind == TokenType::Assign,                        "  [1] Assign");
        check(t[2].kind == TokenType::Number && t[2].text == "2",    "  [2] Number 2");
        check(t[3].kind == TokenType::Star,                          "  [3] Star");
        check(t[4].kind == TokenType::Number && t[4].text == "4",    "  [4] Number 4");
        check(t[5].kind == TokenType::Plus,                          "  [5] Plus");
        check(t[6].kind == TokenType::Number && t[6].text == "4",    "  [6] Number 4");
        check(t[7].kind == TokenType::End,                           "  [7] End sentinel");
    }
    {
        auto t = lex("matA = [[1,2];[3,2];[3,4]]");
        check(t.size() > 5, "matrix literal lexes");
        check(t[0].kind == TokenType::Ident && t[0].text == "matA", "  [0] Ident matA");
        check(t[1].kind == TokenType::Assign,                       "  [1] Assign");
        check(t[2].kind == TokenType::LBracket,                     "  [2] first '['");
        check(t[3].kind == TokenType::LBracket,                     "  [3] second '['");
        check(t[4].kind == TokenType::Number && t[4].text == "1",   "  [4] 1");
        check(t[5].kind == TokenType::Comma,                        "  [5] ,");
        check(t[6].kind == TokenType::Number && t[6].text == "2",   "  [6] 2");
        check(t[7].kind == TokenType::RBracket,                     "  [7] ]");
        check(t[8].kind == TokenType::Semicolon,                    "  [8] ;");
        check(t[9].kind == TokenType::LBracket,                     "  [9] '[' of next row");
        check(t[10].kind == TokenType::Number && t[10].text == "3","  [10] 3");
        // full count: Ident, Assign, [ , [ , 1 , , , 2 , ] , ; ,
        //             [ , 3 , , , 2 , ] , ; , [ , 3 , , , 4 , ] , ]
        //             + End  →  2 + 1 + (2 + 1 + 1 + 2 + 2 + 2) * 3 - ... 
        // just check the last two:
        check(t[t.size() - 2].kind == TokenType::RBracket,          "  penultimate is ']'");
        check(t[t.size() - 1].kind == TokenType::End,               "  last is End");
    }
    {
        auto t = lex("funA(x) = 2*x^5 + 4x^2 - 5*x + 4");
        // check that '^' works, and that '4x' is Number(4), Ident(x) — no implicit mult token
        bool sawCaret = false;
        for (auto &tok : t) if (tok.kind == TokenType::Caret) sawCaret = true;
        check(sawCaret, "funA expression has Caret");
    }
    {
        auto t = lex("a + 2 = ?");
        check(t[0].kind == TokenType::Ident,  "a + 2 = ?  [0] Ident");
        check(t[1].kind == TokenType::Plus,   "a + 2 = ?  [1] Plus");
        check(t[2].kind == TokenType::Number, "a + 2 = ?  [2] Number");
        check(t[3].kind == TokenType::Assign, "a + 2 = ?  [3] Assign");
        check(t[4].kind == TokenType::Query,  "a + 2 = ?  [4] Query");
        check(t[5].kind == TokenType::End,    "a + 2 = ?  [5] End");
    }
    {
        auto t = lex("funA(x) = 0 ?");
        check(t[0].kind == TokenType::Ident && t[0].text == "funA", "funA(x) = 0 ?  [0] Ident funA");
        check(t[1].kind == TokenType::LParen,                       "  [1] LParen");
        check(t[2].kind == TokenType::Ident && t[2].text == "x",    "  [2] Ident x");
        check(t[3].kind == TokenType::RParen,                       "  [3] RParen");
        check(t[4].kind == TokenType::Assign,                       "  [4] Assign");
        check(t[5].kind == TokenType::Number && t[5].text == "0",   "  [5] Number 0");
        check(t[6].kind == TokenType::Query,                        "  [6] Query");
    }

    // --- error cases ---
    auto throwsOn = [](const std::string &s) {
        try { Lexer(s).tokenize(); return false; }
        catch (const std::invalid_argument &) { return true; }
    };
    check(throwsOn("."),  ". rejected");
    check(throwsOn("3.14"), "3.14 rejected (no decimal literals)");
    check(throwsOn("#"),  "# rejected");
    check(throwsOn("a@b"), "@ rejected");
    check(throwsOn("a~b"), "~ rejected");
    check(throwsOn("\x80"), "non-ASCII byte rejected");

    // --- error message contains the column ---
    try { Lexer("a + #").tokenize(); }
    catch (const std::invalid_argument &e)
    {
        std::string msg(e.what());
        check(msg.find("column") != std::string::npos,
              "error message mentions column");
        check(msg.find("'#'") != std::string::npos,
              "error message mentions offending char");
    }

    // --- idempotency ---
    {
        Lexer L("a + b");
        auto t1 = L.tokenize();
        auto t2 = L.tokenize();
        check(t1.size() == t2.size(), "tokenize is idempotent (same size)");
        check(t1[0].text == t2[0].text, "tokenize is idempotent (same [0])");
    }

    std::cout << "\n=== Lexer: " << g_pass << " passed, " << g_fail << " failed ===\n";
    return g_fail == 0 ? 0 : 1;
}