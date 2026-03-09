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
    if(match(TokenType::INT_KEYWORD)) return parseVarDeclarationStatement();

    throw std::runtime_error("No Statment found");
    return nullptr;
}

std::unique_ptr<AST> Parser::parsePrintStatement()
{
    expect(TokenType::PRINT_KEYWORD);
    expect(TokenType::L_PAREN);
    std::unique_ptr<AST> expr = parseExpr();
    expect(TokenType::R_PAREN);
    expect(TokenType::SEMICOLON);
    return std::make_unique<PrintStmtAST>(std::move(expr));
}

std::unique_ptr<AST> Parser::parseVarDeclarationStatement()
{
    Token tk =  expect(TokenType::INT_KEYWORD);
    std::string ident = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::ASSIGN);
    std::unique_ptr<AST> expr = parseExpr();
    expect(TokenType::SEMICOLON);
    return std::make_unique<VarDeclarationStmtAST>(tk.type, ident, std::move(expr));
}

std::unique_ptr<AST> Parser::parseExpr()
{
    std::unique_ptr<AST> left = parseTerm();
    while(match(TokenType::ADD) || match(TokenType::SUB)){
        auto op = advance().type; // current operator ADD or SUB
        auto right = parseTerm();
        left = std::make_unique<BinaryExprAST>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<AST> Parser::parseTerm()
{
    std::unique_ptr<AST> left = parsePrimary();
    while(match(TokenType::MUL) || match(TokenType::DIV)){
        auto op = advance().type; // current operator MUL or DIV
        auto right = parsePrimary();
        left = std::make_unique<BinaryExprAST>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<AST> Parser::parsePrimary()
{
    if(match(TokenType::NUMBER)){
        Token t = expect(TokenType::NUMBER);
        int value = std::stoi(t.value);
        return std::make_unique<IntegerExprAST>(value);
    }
    if(match(TokenType::IDENTIFIER)){
        Token t = expect(TokenType::IDENTIFIER);
        return std::make_unique<VariableExprAST>(t.value);
    }
    if(match(TokenType::L_PAREN)){
        expect(TokenType::L_PAREN);
        auto expr = parseExpr();
        expect(TokenType::R_PAREN);
        return std::move(expr);
    }

    throw std::runtime_error("No Primary found");
    return nullptr;
}