#include "Parser.hpp"

Parser::Parser(std::vector<Token> tokens)
    : tokens_{tokens} {}

bool Parser::match(TokenType type)
{
    return tokens_[pos_].type == type;
}

Token& Parser::advance()
{
    if(pos_ >= tokens_.size()){
        throw std::runtime_error("Out of bound");
    }
    return tokens_[pos_++];
}

Token& Parser::expect(TokenType type)
{
    if(!match(type)){
        std::string expected = Token::typeToString(type);
        std::string actual = Token::typeToString(tokens_[pos_].type);
        throw std::runtime_error("Syntax Error: Expected " + expected + 
                                 ", but found " + actual + " ('" + 
                                 tokens_[pos_].value + "')");
    }
    return advance();
}

std::vector<std::unique_ptr<AST>> Parser::parse()
{
    std::vector<std::unique_ptr<AST>> asts;

    while(tokens_[pos_].type != TokenType::EOF_TOKEN){
        asts.push_back(std::move(parseStatement()));
    }

    expect(TokenType::EOF_TOKEN);
    return asts;
}

std::unique_ptr<AST> Parser::parseStatement()
{
    if(match(TokenType::PRINT_KEYWORD)) return parsePrintStatement();

    throw std::runtime_error("No Statment found");
    return nullptr;
}

std::unique_ptr<AST> Parser::parsePrintStatement()
{
    expect(TokenType::PRINT_KEYWORD);
    expect(TokenType::L_PAREN);
    std::unique_ptr<AST> expr = parsePrimary();
    expect(TokenType::R_PAREN);
    expect(TokenType::SEMICOLON);
    return std::make_unique<PrintStmtAST>(std::move(expr));
}

std::unique_ptr<AST> Parser::parsePrimary()
{
    if(match(TokenType::NUMBER)){
        Token t = expect(TokenType::NUMBER);
        int value = std::stoi(t.value);
        return std::make_unique<IntegerExprAST>(value);
    }

    throw std::runtime_error("No Primary found");
    return nullptr;
}