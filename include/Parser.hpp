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
     * @brief Parse Statement expressions
     */
    std::unique_ptr<AST> parseStatement();

    /**
     * @brief Parse print statment
     * Grammar Rule: PrintStmt -> 'print' '(' Expression ')' ';'
     */
    std::unique_ptr<AST> parsePrintStatement();

    /**
     * @brief Pasrse Variable Declaration (int)
     * Grammar Rule: VarDeclrStmt -> 'int' IDENTIFIER '=' Expression ';'
     */
    std::unique_ptr<AST> parseVarDeclarationStatement();

    /**
     * @brief Parse expression ( ADD /SUB )
     */
    std::unique_ptr<AST> parseExpr();

    /**
     * @brief Parse term ( MUL / DIV )
     */
    std::unique_ptr<AST> parseTerm();


    /**
     * @brief Grammar Rule: Primary -> Integer
     */
    std::unique_ptr<AST> parsePrimary();

public:
    Parser(std::vector<Token> tokens);

    /**
     * @brief Main entry point to turn tokens into a list of AST nodes
     */
    std::vector<std::unique_ptr<AST>> parse();
};