#include "Parser.hpp"

Parser::Parser(std::vector<Token> tokens)
    : tokens_{tokens} {}

bool Parser::match(TokenType type)
{
    return tokens_[pos_].type == type;
}


Token& Parser::peek()
{
    if (pos_ + 1 >= tokens_.size()) return tokens_.back(); // Return EOF if at limit
    return tokens_[pos_ + 1];
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
        asts.push_back(std::move(parseFunctionDeclaration()));
    }

    expect(TokenType::EOF_TOKEN);
    return asts;
}

// Structural Parsing 
std::unique_ptr<AST> Parser::parseFunctionDeclaration()
{
    expect(TokenType::FN_KEYWORD);
    auto functionName = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::L_PAREN);
    auto args = parseParams();
    expect(TokenType::R_PAREN);
    expect(TokenType::COLON);
    auto returnType = advance().type;
    auto body = parseBlock();
    return std::make_unique<FunctionAST>(functionName, std::move(args), returnType, std::move(body));
}

// Handles the ( ) and comma loop
std::vector<std::pair<TokenType, std::string>> Parser::parseParams()
{
    std::vector<std::pair<TokenType, std::string>> params;
    if(!match(TokenType::R_PAREN)){
        do {
            params.push_back(parseParam());
        } while (match(TokenType::COMMA) && advance().type == TokenType::COMMA);
    }
    return params;
}

// Handles a single "int x"
std::pair<TokenType, std::string> Parser::parseParam()
{
    auto type = expect(TokenType::INT_KEYWORD).type;
    auto identifier = expect(TokenType::IDENTIFIER).value;
    return {type, identifier};
}

// Handles the { statements }
std::unique_ptr<BlockAST> Parser::parseBlock()
{
    std::vector<std::unique_ptr<AST>> statements;
    expect(TokenType::L_CURL);
    while(tokens_[pos_].type != TokenType::R_CURL){
        statements.push_back(std::move(parseStatement()));
    }
    expect(TokenType::R_CURL);
    return std::make_unique<BlockAST>(std::move(statements));
}

std::unique_ptr<AST> Parser::parseStatement()
{
    if(match(TokenType::INT_KEYWORD)) return parseVarDeclarationStatement();
    if(match(TokenType::IDENTIFIER)) {
        if(peek().type == TokenType::ASSIGN) return parseAssignmentStatement();
        if(peek().type == TokenType::L_PAREN) return parseFunctionCall();
    }
    if(match(TokenType::PRINT_KEYWORD)) return parsePrintStatement();
    if(match(TokenType::IF_KEYWORD)) return parseIfStatement();
    if(match(TokenType::WHILE_KEYWORD)) return parseWhileStatement();
    if(match(TokenType::RETURN_KEYWORD)) return parseReturnStatement();

    throw std::runtime_error("No Statment found");
    return nullptr;
}

std::unique_ptr<AST> Parser::parseVarDeclarationStatement()
{
    Token tk =  expect(TokenType::INT_KEYWORD);
    std::string ident = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::ASSIGN);
    std::unique_ptr<AST> expr = parseExpression();
    expect(TokenType::SEMICOLON);
    return std::make_unique<VarDeclarationStmtAST>(tk.type, ident, std::move(expr));
}

std::unique_ptr<AST> Parser::parseAssignmentStatement()
{
    std::string ident = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::ASSIGN);
    std::unique_ptr<AST> expr = parseExpression();
    expect(TokenType::SEMICOLON);
    return std::make_unique<AssignmentStmtAST>(ident, std::move(expr));
}

std::unique_ptr<AST> Parser::parsePrintStatement()
{
    expect(TokenType::PRINT_KEYWORD);
    expect(TokenType::L_PAREN);
    std::unique_ptr<AST> expr = parseExpression();
    expect(TokenType::R_PAREN);
    expect(TokenType::SEMICOLON);
    return std::make_unique<PrintStmtAST>(std::move(expr));
}

std::unique_ptr<AST> Parser::parseIfStatement()
{
    expect(TokenType::IF_KEYWORD);
    expect(TokenType::L_PAREN);
    auto condition = parseExpression();
    expect(TokenType::R_PAREN);
    auto thenBlock = parseBlock();
    std::unique_ptr<BlockAST> elseBlock = nullptr;
    if(match(TokenType::ELSE_KEYWORD)){
        expect(TokenType::ELSE_KEYWORD);
        elseBlock = parseBlock();
    }
    return std::make_unique<IfStmtAST>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

std::unique_ptr<AST> Parser::parseWhileStatement()
{
    expect(TokenType::WHILE_KEYWORD);
    expect(TokenType::L_PAREN);
    std::unique_ptr<AST> condition = parseExpression();
    expect(TokenType::R_PAREN);
    std::unique_ptr<BlockAST> block = parseBlock();
    return std::make_unique<WhileStmtAST>(std::move(condition), std::move(block));
}

std::unique_ptr<AST> Parser::parseReturnStatement()
{
    expect(TokenType::RETURN_KEYWORD);
    std::unique_ptr<AST> expression = parseExpression();
    expect(TokenType::SEMICOLON);
    return std::make_unique<ReturnStmtAST>(std::move(expression));
}


std::unique_ptr<AST> Parser::parseExpression()
{
    std::unique_ptr<AST> expr = parseComparison();
    return std::move(expr);
}

std::unique_ptr<AST> Parser::parseComparison()
{
    std::unique_ptr<AST> left = parseAdditive();
    if(match(TokenType::EQ)     || match(TokenType::NOT_EQ) || 
       match(TokenType::GT)     || match(TokenType::LT)     ||
       match(TokenType::GE)     || match(TokenType::LE)){
        auto op = advance().type;
        auto right = parseAdditive();
        left = std::make_unique<BinaryExprAST>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<AST> Parser::parseAdditive()
{
    std::unique_ptr<AST> left = parseMultiplicative();
    while(match(TokenType::ADD) || match(TokenType::SUB)){
        auto op = advance().type; // current operator ADD or SUB
        auto right = parseMultiplicative();
        left = std::make_unique<BinaryExprAST>(op, std::move(left), std::move(right));
    }
    return left;
}


std::unique_ptr<AST> Parser::parseMultiplicative()
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
        if(peek().type == TokenType::L_PAREN) return parseFunctionCall();
        Token t = expect(TokenType::IDENTIFIER);
        return std::make_unique<VariableExprAST>(t.value);
    }
    if(match(TokenType::L_PAREN)){
        expect(TokenType::L_PAREN);
        auto expr = parseExpression();
        expect(TokenType::R_PAREN);
        return std::move(expr);
    }

    throw std::runtime_error("No Primary found");
    return nullptr;
}


std::unique_ptr<AST> Parser::parseFunctionCall()
{
    auto callee = expect(TokenType::IDENTIFIER).value;
    expect(TokenType::L_PAREN);
    auto args = parseArgs();
    expect(TokenType::R_PAREN);
    return std::make_unique<CallExprAST>(callee, std::move(args));
}

std::vector<std::unique_ptr<AST>> Parser::parseArgs()
{
    std::vector<std::unique_ptr<AST>> args;
    if (!match(TokenType::R_PAREN)) {
        do {
            args.push_back(parseExpression());
        } while (match(TokenType::COMMA) && advance().type == TokenType::COMMA);
    }
    return args;
}