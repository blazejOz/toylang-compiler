#pragma once

#include <string>

/** @brief Categories of tokens supported by the compiler. */
enum class TokenType
{
    // Keywords
    PRINT_KEYWORD,
    INT_KEYWORD,
    VOID_KEYWORD,
    IF_KEYWORD,
    ELSE_KEYWORD,
    WHILE_KEYWORD,
    RETURN_KEYWORD,
    FN_KEYWORD,
    
    // Identifiers and Literals
    IDENTIFIER,
    NUMBER,

    //Comparision Operatoes
    EQ,       // ==
    NOT_EQ,   // !=
    GT,       // >
    LT,       // <
    GE,       // >= 
    LE,       // <=

    // Arithmetic Operators
    ADD,      // +
    SUB,      // -
    MUL,      // *
    DIV,      // /
    
    // Symbols
    L_CURL,    // {
    R_CURL,    // }
    L_PAREN,   // (
    R_PAREN,   // )
    SEMICOLON, // ;
    ASSIGN,    // =
    COLON,     // :
    COMMA,     // ,

    // Special
    UNKNOWN,
    EOF_TOKEN
};

/** @brief Represents a single unit of source code. */
struct Token
{
    TokenType type;
    std::string value;

    /** @brief Converts a TokenType to a human-readable string for error reporting. */
    static std::string typeToString(TokenType type) 
    {
        switch(type) {
            // Keywords
            case TokenType::FN_KEYWORD:     return "keyword 'fn'";
            case TokenType::INT_KEYWORD:    return "keyword 'int'";
            case TokenType::VOID_KEYWORD:   return "keyword 'void'";
            case TokenType::PRINT_KEYWORD:  return "keyword 'print'";
            case TokenType::IF_KEYWORD:     return "keyword 'if'";
            case TokenType::ELSE_KEYWORD:   return "keyword 'else'";
            case TokenType::WHILE_KEYWORD:  return "keyword 'while'";
            case TokenType::RETURN_KEYWORD: return "keyword 'return'";

            // Identifiers/Literals
            case TokenType::IDENTIFIER:     return "identifier";
            case TokenType::NUMBER:         return "integer literal";
            
            // Arithmetic
            case TokenType::ADD:            return "+";
            case TokenType::SUB:            return "-";
            case TokenType::MUL:            return "*";
            case TokenType::DIV:            return "/";
            
            // Comparisons
            case TokenType::EQ:             return "==";
            case TokenType::NOT_EQ:         return "!=";
            case TokenType::GT:             return ">";
            case TokenType::LT:             return "<";
            case TokenType::GE:             return ">=";
            case TokenType::LE:             return "<=";

            // Symbols
            case TokenType::L_PAREN:        return "(";
            case TokenType::R_PAREN:        return ")";
            case TokenType::L_CURL:         return "{";
            case TokenType::R_CURL:         return "}";
            case TokenType::SEMICOLON:      return ";";
            case TokenType::ASSIGN:         return "=";
            case TokenType::COLON:          return ":";
            case TokenType::COMMA:          return ",";
            
            // Special
            case TokenType::UNKNOWN:        return "unknown character";
            case TokenType::EOF_TOKEN:      return "end of file";
            
            default:                        return "unrecognized token type";
        }
    }
};