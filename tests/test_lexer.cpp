#include <catch2/catch_test_macros.hpp>
#include "Lexer.hpp"

TEST_CASE("Lexer skips white spaces", "[lexer]")
{
    Lexer lexer{"     a   "};

    std::vector tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0].type == TokenType::IDENTIFIER);
    REQUIRE(tokens[1].type == TokenType::EOF_TOKEN);
}