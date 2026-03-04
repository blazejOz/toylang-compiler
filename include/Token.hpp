#pragma once

#include <string>


enum class TokenType
{
    //KEY WORDS
    PRINT_KEYWORD,
    
    
    IDENTIFIER,
    
    //LITERALS
    NUMBER,
    STRING,
    
    L_PAREN,
    R_PAREN,
    SEMICOLON,

    UNKNOWN,
    EOF_TOKEN
};

struct Token
{
    TokenType type;
    std::string value;

    static std::string typeToString(TokenType type) 
    {
        switch(type) {
            case TokenType::PRINT_KEYWORD: return "'print'";
            case TokenType::L_PAREN:       return "'('";
            case TokenType::R_PAREN:       return "')'";
            case TokenType::SEMICOLON:     return "';'";
            case TokenType::NUMBER:        return "integer literal";
            case TokenType::IDENTIFIER:    return "identifier";
            case TokenType::UNKNOWN:       return "unknown character";
            case TokenType::EOF_TOKEN:     return "end of file";
            default:                       return "unrecognized token type";
        }
    }
};

