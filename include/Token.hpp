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
    
    EOF_TOKEN,

    L_PAREN,
    R_PAREN,
    SEMICOLON
};

struct Token
{
    TokenType type;
    std::string value;
};
