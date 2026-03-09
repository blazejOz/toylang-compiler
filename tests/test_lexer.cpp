#include <catch2/catch_test_macros.hpp>
#include "Lexer.hpp"

TEST_CASE("Lexer skips white spaces and handles identifiers", "[lexer]")
{
    Lexer lexer{"    ala   print"};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(tokens[1].type == TokenType::PRINT_KEYWORD);
    CHECK(tokens[2].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer skips all white spaces", "[lexer]")
{
    Lexer lexer{" \n  \t   "};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer handles single chars", "[lexer]")
{
    Lexer lexer{" (  )  ;  & "};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 5);
    CHECK(tokens[0].type == TokenType::L_PAREN);
    CHECK(tokens[1].type == TokenType::R_PAREN);
    CHECK(tokens[2].type == TokenType::SEMICOLON);
    CHECK(tokens[3].type == TokenType::UNKNOWN);
    CHECK(tokens[4].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer handles arithmetic without spaces", "[lexer]")
{
    Lexer lexer{"1+2*3;"};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 7); // 1, +, 2, *, 3, ;, EOF
    CHECK(tokens[0].type == TokenType::NUMBER);
    CHECK(tokens[1].type == TokenType::ADD);
    CHECK(tokens[2].type == TokenType::NUMBER);
    CHECK(tokens[3].type == TokenType::MUL);
    CHECK(tokens[4].type == TokenType::NUMBER);
    CHECK(tokens[5].type == TokenType::SEMICOLON);
    CHECK(tokens[6].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer handles greedy keywords", "[lexer]")
{
    // 'print' is a keyword, but 'printed' is an identifier
    Lexer lexer{"printed"};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 2);
    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(tokens[0].value == "printed");
}