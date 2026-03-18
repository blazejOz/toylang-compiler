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

//Navigation Utils

    /** @brief Checks if the current token matches the expected type. */
    bool match(TokenType type);

    /** @brief Consumes the current token and moves to the next one. */
    Token& advance();

    /** @brief Validates that the current token is of the expected type, then advances. 
     * Throws an error if the type does not match. */
    Token& expect(TokenType type);

//Structural Parsing (Functions & Blocks)

    /** @brief Rule: fn identifier(params): type { block } */
    std::unique_ptr<AST> parseFunctionDeclaration();

    /** @brief Parses a comma-separated list of parameters: (int a, int b) */
    std::vector<std::pair<TokenType, std::string>> parseParams();

    /** @brief Parses a single parameter unit: int a */
    std::pair<TokenType, std::string> parseParam();

    /** @brief Parses a series of statements enclosed in braces: { ... } */
    std::unique_ptr<BlockAST> parseBlock();

//Statement Parsing

    /** @brief Main statement router. Calls specific statement types. */
    std::unique_ptr<AST> parseStatement();

    /** @brief Rule: type identifier = expression; */
    std::unique_ptr<AST> parseVarDeclarationStatement();

    /** @brief Rule: identifier = expression; */
    std::unique_ptr<AST> parseAssignmentStatement();

    /** @brief Rule: print(expression); */
    std::unique_ptr<AST> parsePrintStatement();

    /** @brief Rule: if (expression) { block } [else { block }] */
    std::unique_ptr<AST> parseIfStatement();

    /** @brief Rule: while (expression) { block } */
    std::unique_ptr<AST> parseWhileStatement();

    /** @brief Rule: return [expression]; */
    std::unique_ptr<AST> parseReturnStatement();

//Expression Parsing

    /** @brief Entry point for expressions. Matches lowest precedence (Comparisons). */
    std::unique_ptr<AST> parseExpression();

    /** @brief Handles equality and relational operators: ==, !=, <, >, <=, >= */
    std::unique_ptr<AST> parseComparison();

    /** @brief Handles addition and subtraction: +, - */
    std::unique_ptr<AST> parseAdditive();

    /** @brief Handles multiplication and division: *, / */
    std::unique_ptr<AST> parseMultiplicative();

    /** @brief Handles the highest precedence units: Numbers, Identifiers, and Parentheses. */
    std::unique_ptr<AST> parsePrimary();

    /** @brief Helper to parse a function call when an identifier is followed by '('. */
    std::unique_ptr<AST> parseFunctionCall();

    /** @brief Parses a comma-separated list of arguments for a function call. */
    std::vector<std::unique_ptr<AST>> parseArgs();

public:
    Parser(std::vector<Token> tokens);

    /**
     * @brief Parses the entire token stream into a list of top-level nodes (usually functions).
     */
    std::vector<std::unique_ptr<AST>> parse();
};