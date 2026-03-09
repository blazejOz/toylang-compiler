#pragma once

#include <string>

/**
 * @brief Categories of tokens supported by the compiler.
 */
enum class TokenType
{
    // Keywords
    PRINT_KEYWORD,
    INT_KEYWORD,
    
    // Identifiers and Literals
    IDENTIFIER,
    NUMBER,
    STRING,
    
    // Arithmetic Operators
    ADD,      // +
    SUB,      // -
    MUL,      // *
    DIV,      // /
    
    // Symbols
    L_PAREN,   // (
    R_PAREN,   // )
    SEMICOLON, // ;
    ASSIGN,    // =

    // Special
    UNKNOWN,
    EOF_TOKEN
};

/**
 * @brief Represents a single unit of source code.
 */
struct Token
{
    TokenType type;
    std::string value;

    /**
     * @brief Converts a TokenType to a human-readable string for error reporting.
     */
    static std::string typeToString(TokenType type) 
    {
        switch(type) {
            // Keywords
            case TokenType::PRINT_KEYWORD: return "keyword 'print'";
            case TokenType::INT_KEYWORD: return "keyword 'int'";

            
            // Identifiers/Literals
            case TokenType::IDENTIFIER:    return "identifier";
            case TokenType::NUMBER:        return "integer literal";
            case TokenType::STRING:        return "string literal";
            
            // Arithmetic
            case TokenType::ADD:           return "+";
            case TokenType::SUB:           return "-";
            case TokenType::MUL:           return "*";
            case TokenType::DIV:           return "/";
            
            // Symbols
            case TokenType::L_PAREN:       return "(";
            case TokenType::R_PAREN:       return ")";
            case TokenType::SEMICOLON:     return ";";
            case TokenType::ASSIGN:        return "=";
            
            // Special
            case TokenType::UNKNOWN:       return "unknown character";
            case TokenType::EOF_TOKEN:     return "end of file";
            
            default:                       return "unrecognized token type";
        }
    }
};