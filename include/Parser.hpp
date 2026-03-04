#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "Token.hpp"
#include "AST.hpp"

class Parser {
private:
    std::vector<Token> tokens_;
    size_t pos_ = 0;

    /**
     * @brief compare type to curr token type
     */
    bool match(TokenType type);

    /**
     * @brief Consume and return the current token
     */
    Token& advance();

    /**
     * @brief Check if current token matches expected type; if so, advance.
     */
    Token& expect(TokenType type);

    /**
     * @brief Grammar Rule: Primary -> Integer
     */
    std::unique_ptr<AST> parsePrimary();

    /**
     * @brief Grammar Rule: PrintStmt -> 'print' '(' Expression ')' ';'
     */
    std::unique_ptr<AST> parsePrintStatement();

    /**
     * @brief Parse Statement expressions
     */
    std::unique_ptr<AST> parseStatement();
public:
    Parser(std::vector<Token> tokens);

    /**
     * @brief Main entry point to turn tokens into a list of AST nodes
     */
    std::vector<std::unique_ptr<AST>> parse();
};