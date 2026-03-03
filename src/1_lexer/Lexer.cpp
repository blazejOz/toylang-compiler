#include "Lexer.hpp"

Lexer::Lexer(std::string source_code): input_{source_code}
{
    keywords_["print"] = TokenType::PRINT_KEYWORD;
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    Token t = getNextToken();
    while(t.type != TokenType::EOF_TOKEN){
        tokens.push_back(t);
        t = getNextToken();
    }
    tokens.push_back(t); // Add EOF token
    return tokens;
}

char Lexer::peek() const
{
    if(pos_ >= input_.length()) return '\0';
    return input_[pos_];
}

char Lexer::advance()
{
    if(pos_ >= input_.length()) return '\0';
    return input_[pos_++];
}

Token Lexer::getNextToken()
{
    char curr = ' ';
    
    while(isspace(curr)){
        curr = advance();
    }
    
    if(isalpha(curr)){
        std::string word;
        word += curr;
        while(isalnum(curr = advance())){
            word += curr;
        }

        if(keywords_.contains(word)) return {keywords_[word], word};

        return {TokenType::IDENTIFIER, word};
    }

    return {TokenType::EOF_TOKEN, "\0"};
}