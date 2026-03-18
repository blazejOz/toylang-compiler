#include <catch2/catch_test_macros.hpp>
#include "Lexer.hpp"


TEST_CASE("Lexer skips all white spaces", "[lexer]")
{
    Lexer lexer{" \n  \t   "};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer identifies keywords", "[lexer]")
{
    Lexer lexer{" print int void if else while return fn "};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 9);
    CHECK(tokens[0].type == TokenType::PRINT_KEYWORD);
    CHECK(tokens[1].type == TokenType::INT_KEYWORD);
    CHECK(tokens[2].type == TokenType::VOID_KEYWORD);
    CHECK(tokens[3].type == TokenType::IF_KEYWORD);
    CHECK(tokens[4].type == TokenType::ELSE_KEYWORD);
    CHECK(tokens[5].type == TokenType::WHILE_KEYWORD);
    CHECK(tokens[6].type == TokenType::RETURN_KEYWORD);
    CHECK(tokens[7].type == TokenType::FN_KEYWORD);
    CHECK(tokens[8].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer identifies comparison operators", "[lexer]")
{
    Lexer lexer{" == != > < >= <=   "};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 7);
    CHECK(tokens[0].type == TokenType::EQ);
    CHECK(tokens[1].type == TokenType::NOT_EQ);
    CHECK(tokens[2].type == TokenType::GT);
    CHECK(tokens[3].type == TokenType::LT);
    CHECK(tokens[4].type == TokenType::GE);
    CHECK(tokens[5].type == TokenType::LE);
    CHECK(tokens[6].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer identifies single chars", "[lexer]")
{
    Lexer lexer{" { } (  )  ;  = : ,  ? "};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 10);
    CHECK(tokens[0].type == TokenType::L_CURL);
    CHECK(tokens[1].type == TokenType::R_CURL);
    CHECK(tokens[2].type == TokenType::L_PAREN);
    CHECK(tokens[3].type == TokenType::R_PAREN);
    CHECK(tokens[4].type == TokenType::SEMICOLON);
    CHECK(tokens[5].type == TokenType::ASSIGN);
    CHECK(tokens[6].type == TokenType::COLON);
    CHECK(tokens[7].type == TokenType::COMMA);
    CHECK(tokens[8].type == TokenType::UNKNOWN);
    CHECK(tokens[9].type == TokenType::EOF_TOKEN);
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

TEST_CASE("Lexer handles a realistic function signature", "[lexer]")
{
    Lexer lexer{"fn add(int a, int b): int"};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 12);
    CHECK(tokens[0].type == TokenType::FN_KEYWORD);
    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(tokens[2].type == TokenType::L_PAREN);
    CHECK(tokens[3].type == TokenType::INT_KEYWORD);
    CHECK(tokens[4].type == TokenType::IDENTIFIER);
    CHECK(tokens[5].type == TokenType::COMMA);
    CHECK(tokens[6].type == TokenType::INT_KEYWORD);
    CHECK(tokens[7].type == TokenType::IDENTIFIER);
    CHECK(tokens[8].type == TokenType::R_PAREN);
    CHECK(tokens[9].type == TokenType::COLON);
    CHECK(tokens[10].type == TokenType::INT_KEYWORD);
    CHECK(tokens[11].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer handles identifiers with numbers", "[lexer]")
{
    Lexer lexer{"var123 x1y2"};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].value == "var123");
    CHECK(tokens[1].value == "x1y2");
    CHECK(tokens[2].type == TokenType::EOF_TOKEN);
}

TEST_CASE("Lexer handles triple equals", "[lexer]")
{
    Lexer lexer{"==="};
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].type == TokenType::EQ);     // ==
    CHECK(tokens[1].type == TokenType::ASSIGN); // =
    CHECK(tokens[2].type == TokenType::EOF_TOKEN);
}