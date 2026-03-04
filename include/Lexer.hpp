#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Token.hpp"

/**
 * @brief Performs lexical analysis to transform source code into a stream of tokens.
 */
class Lexer
{
private:
    std::string input_;
    size_t pos_ = 0;
    std::unordered_map<std::string, TokenType> keywords_;

    /**
     * @brief Returns the character at the current position without advancing.
     */
    char peek() const;

    /**
     * @brief Consumes the current character and moves the position pointer forward.
     * @return The character that was consumed.
     */
    char advance();

    /**
     * @brief Scans the input to identify the next individual token.
     * @return The identified Token object.
     */
    Token getNextToken();

public:
    /**
     * @brief Constructs a Lexer with the provided source code.
     * @param source_code The raw source text to be tokenized.
     */
    explicit Lexer(std::string source_code);

    /**
     * @brief Iteratively scans the entire input to produce a complete token stream.
     * @return A vector of Token objects ending with an EOF token.
     */
    std::vector<Token> tokenize();
};