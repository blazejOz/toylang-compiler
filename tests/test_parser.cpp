#include <catch2/catch_test_macros.hpp>
#include "Lexer.hpp"
#include "Parser.hpp"

class ParserTester {
public:
    static std::unique_ptr<AST> parseExpr(Parser& p) { return p.parseExpression(); }
    static std::unique_ptr<AST> parseStmt(Parser& p) { return p.parseStatement(); }
};

struct TestCase {
    std::string input;          
    std::string expected_match; 
};

TEST_CASE("Parser handels function declarations" , "[parser]")
{
    std::vector<TestCase> cases = {
        {"fn f(): void {}", "fn f"},
        {"fn add(int a): int { return a; }", "(return a)"},
        {"fn main(): int { print(1); }", "(print 1)"}
    };

    for (const auto& c : cases) {
        Parser parser{Lexer{c.input}.tokenize()};
        auto ast = parser.parse();
        CHECK(ast[0]->toString().find(c.expected_match) != std::string::npos);
    }
}

TEST_CASE("Parser handles IF WHILE PRINT statements", "[parser]")
{
    std::vector<TestCase> cases = {
        //Print stmt
        { "print(42);", "(print 42)" },

        //If 
        { "if (x > 0) { print(1); }", "(if (> x 0) { (print 1) })" },

        //While
        { "while (i < 10) { i = i + 1; }", "(while (< i 10) { (assign i (+ i 1)) })" },

        //Return 
        { "return x + 5;", "(return (+ x 5))" }
    };

    for (const auto& c : cases) {
        CAPTURE(c.input);
        Lexer lexer{c.input};
        Parser parser{lexer.tokenize()};

        auto stmt = ParserTester::parseStmt(parser);

        REQUIRE(stmt != nullptr);
        CHECK(stmt->toString().find(c.expected_match) != std::string::npos);
    }
}


TEST_CASE("Parser handles expressions", "[parser]")
{
std::vector<TestCase> cases = {
        //Basic Arithmetic
        { "10 + 20", "(+ 10 20)" },
        { "50 - 5", "(- 50 5)" },
        { "4 * 2", "(* 4 2)" },
        { "10 / 2", "(/ 10 2)" },

        //Operator Precedence
        { "2 + 3 * 4", "(+ 2 (* 3 4))" },
        { "2 * 3 + 4", "(+ (* 2 3) 4)" },

        //Parentheses
        { "(2 + 3) * 4", "(* (+ 2 3) 4)" },

        //Comparison Operators
        { "x > 10", "(> x 10)" },
        { "y < 5", "(< y 5)" },
        { "a == b", "(== a b)" },
        { "a != b", "(!= a b)" },

        //Logical/Complex Nesting
        { "x > 0 + 5", "(> x (+ 0 5))" }, 
        { "10 + 20 * 30 / 40", "(+ 10 (/ (* 20 30) 40))" } 
    };

    for (const auto& c : cases) {
        CAPTURE(c.input);
        Lexer lexer{c.input};
        Parser parser{lexer.tokenize()};

        auto expr = ParserTester::parseExpr(parser);
        
        REQUIRE(expr != nullptr);
        CHECK(expr->toString().find(c.expected_match) != std::string::npos);
    }
}