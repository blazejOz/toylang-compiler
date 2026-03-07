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

TEST_CASE("Parser builds var decl with Integer", "[parser]") {
    Lexer lexer{"int x = 2; print(x);"};
    Parser parser{lexer.tokenize()};

    auto asts = parser.parse();

    REQUIRE(asts.size() == 2);
    REQUIRE(dynamic_cast<VarDeclarationStmtAST*>(asts[0].get()) != nullptr);
    REQUIRE(dynamic_cast<PrintStmtAST*>(asts[1].get()) != nullptr);
}