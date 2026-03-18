#include "Lexer.hpp"

Lexer::Lexer(std::string source_code): input_{source_code}
{
    keywords_ = {
        {"fn",     TokenType::FN_KEYWORD},
        {"int",    TokenType::INT_KEYWORD},
        {"void",   TokenType::VOID_KEYWORD},
        {"print",  TokenType::PRINT_KEYWORD},
        {"if",     TokenType::IF_KEYWORD},
        {"else",   TokenType::ELSE_KEYWORD},
        {"while",  TokenType::WHILE_KEYWORD},
        {"return", TokenType::RETURN_KEYWORD}
    };
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
    //skip white space
    while(isspace(peek())){
        advance();
    }
    
    char curr = peek();

    //Handle Identifiers and Keywords
    if(isalpha(curr)){
        std::string word;
        while(isalnum(peek())){
            word += advance();
        }

        if(keywords_.contains(word)) return {keywords_[word], word};

        return {TokenType::IDENTIFIER, word};
    }

    //Handle numbers
    if (isdigit(curr)) {
        std::string num;
        while (isdigit(peek())) {
            num += advance();
        }
        return {TokenType::NUMBER, num};
    }

    //Handle Comparison Operators
    char c = advance();
    
    if(c == '='){
        if(peek() == '='){
            advance();
            return {TokenType::EQ, "=="};
        }
    }
    
    if(c == '!'){
        if(peek() == '='){
            advance();
            return {TokenType::NOT_EQ, "!="};
        }
    }
    
    if(c == '>'){
        if(peek() == '='){
            advance();
            return {TokenType::GE, ">="};
        }else{
            return {TokenType::GT, ">"};
        }
    }
    
    if(c == '<'){
        if(peek() == '='){
            advance();
            return {TokenType::LE, "<="};
        }else{
            return {TokenType::LT, "<"};
        }
    }
    
    // Handle Single Characters
    if (c == '{') return {TokenType::L_CURL, "{"};
    if (c == '}') return {TokenType::R_CURL, "}"};
    if (c == '(') return {TokenType::L_PAREN, "("};
    if (c == ')') return {TokenType::R_PAREN, ")"};
    if (c == ';') return {TokenType::SEMICOLON, ";"};
    if (c == '=') return {TokenType::ASSIGN, "="};
    if (c == ':') return {TokenType::COLON, ":"};
    if (c == ',') return {TokenType::COMMA, ","};

    //ARITHMETIC
    if (c == '+') return {TokenType::ADD, "+"};
    if (c == '-') return {TokenType::SUB, "-"};
    if (c == '*') return {TokenType::MUL, "*"};
    if (c == '/') return {TokenType::DIV, "/"};

    
    if(c == '\0') return {TokenType::EOF_TOKEN, ""};

    // If reached return UNKNOWN
    return {TokenType::UNKNOWN, std::string(1, c)};
}