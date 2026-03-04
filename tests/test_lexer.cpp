#include <catch2/catch_test_macros.hpp>
#include "Lexer.hpp"

TEST_CASE("Lexer skips white spaces and handles identifiers", "[lexer]")
{
    Lexer lexer{"     a   print"};

    std::vector tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].type == TokenType::PRINT_KEYWORD);
    REQUIRE(tokens[2].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer skips all white spaces", "[lexer]")
{
    Lexer lexer{"   "};

    std::vector tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer handles single chars", "[lexer]")
{
    Lexer lexer{" (  )  ;  &"};

    std::vector tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 5);
    REQUIRE(tokens[0].type == TokenType::L_PAREN);
    REQUIRE(tokens[1].type == TokenType::R_PAREN);
    REQUIRE(tokens[2].type == TokenType::SEMICOLON);
    REQUIRE(tokens[3].type == TokenType::UNKNOWN);
    REQUIRE(tokens[4].type == TokenType::EOF_TOKEN);
}
