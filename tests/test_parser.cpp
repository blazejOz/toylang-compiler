#include <catch2/catch_test_macros.hpp>
#include "Lexer.hpp"
#include "Parser.hpp"

TEST_CASE("Parser builds PrintStmt with Integer", "[parser]") {
    Lexer lexer{"print(42);"};
    Parser parser{lexer.tokenize()};

    auto asts = parser.parse();

    REQUIRE(asts.size() == 1);
    REQUIRE(dynamic_cast<PrintStmtAST*>(asts[0].get()) != nullptr);
}

TEST_CASE("Parser handles variable declarations and usage", "[parser]") {
    Lexer lexer{"int speed = 50 + 5; print(speed);"};
    Parser parser{lexer.tokenize()};
    auto asts = parser.parse();

    REQUIRE(asts.size() == 2);
    
    // Check declaration
    CHECK(asts[0]->toString() == "(decl speed (+ 50 5))");
    
    // Check print
    CHECK(asts[1]->toString() == "(print speed)");
}

TEST_CASE("Parser handles operator precedence (PEMDAS)", "[parser]") {
    Lexer lexer{"print(2 + 2 * 10);"};
    Parser parser{lexer.tokenize()};
    auto asts = parser.parse();

    REQUIRE(asts.size() == 1);
    CHECK(asts[0]->toString() == "(print (+ 2 (* 2 10)))");
}

TEST_CASE("Parser respects parentheses for overriding precedence", "[parser]") {
    Lexer lexer{"print((2 + 2) * 10);"};
    Parser parser{lexer.tokenize()};
    auto asts = parser.parse();

    REQUIRE(asts.size() == 1);
    CHECK(asts[0]->toString() == "(print (* (+ 2 2) 10))");
}

TEST_CASE("Parser throws error on missing semicolon", "[parser]") {
    Lexer lexer{"print(42)"}; // missing ;
    Parser parser{lexer.tokenize()};

    REQUIRE_THROWS(parser.parse());
}

TEST_CASE("Parser throws error on unbalanced parentheses", "[parser]") {
    Lexer lexer{"print(2 + (3 * 4);"}; // missing )
    Parser parser{lexer.tokenize()};

    REQUIRE_THROWS(parser.parse());
}