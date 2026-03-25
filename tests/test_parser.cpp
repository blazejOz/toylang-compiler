#include <catch2/catch_test_macros.hpp>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"

/** @brief Helper class to access protected/private parser methods */
class ParserTester {
public:
    static std::unique_ptr<AST> parseExpr(Parser& p) { return p.parseExpression(); }
    static std::unique_ptr<AST> parseStmt(Parser& p) { return p.parseStatement(); }
};

/** @brief Structural Helper: Casts a node and fails the test if it's the wrong type */
template <typename T>
T* require_node(AST* node) 
{
    REQUIRE(node != nullptr);
    auto* casted = dynamic_cast<T*>(node);
    REQUIRE(casted != nullptr); 
    return casted;
}

TEST_CASE("Parser handles Function definitions", "[parser]")
{
    SECTION("Basic function definition: fn f(): void {}") {
        Parser p{Lexer{"fn f(): void {}"}.tokenize()};
        auto module = p.parse();
        
        REQUIRE(!module.empty());
        auto* fnNode = require_node<FunctionAST>(module[0].get());
        
        CHECK(fnNode->getName() == "f");
        CHECK(fnNode->getReturnType() == "keyword 'void'");
        CHECK(fnNode->getArgs().size() == 0);
    }

    SECTION("Function with arguments: fn add(int a, int b): int") {
        Parser p{Lexer{"fn add(int a, int b): int { }"}.tokenize()};
        auto module = p.parse();
        
        auto* fnNode = require_node<FunctionAST>(module[0].get());
        auto args = fnNode->getArgs();
        
        REQUIRE(args.size() == 2);
        CHECK(args[0].second == "a"); // Name of first arg
        CHECK(args[1].second == "b"); // Name of second arg
        CHECK(fnNode->getReturnType() == "keyword 'int'");
    }

    SECTION("Function body with return: fn g(): int { return 5; }") {
        Parser p{Lexer{"fn g(): int { return 5; }"}.tokenize()};
        auto module = p.parse();
        
        auto* fnNode = require_node<FunctionAST>(module[0].get());
        auto* body = fnNode->getBody();
        
        // BlockAST returns a vector of statements
        REQUIRE(body->getStatements().size() == 1);
        
        auto* ret = require_node<ReturnStmtAST>(body->getStatements()[0].get());
        auto* val = require_node<IntegerExprAST>(ret->getExpression());
        CHECK(val->getValue() == 5);
    }

}

TEST_CASE("Parser handles statemants" , "[parser]")
{
    SECTION("Simple variable declaration statement: int x = 42;") {
        Parser p{Lexer{"int x = 42;"}.tokenize()};
        auto stmt = ParserTester::parseStmt(p);
        
        auto* declNode = require_node<VarDeclarationStmtAST>(stmt.get());
        CHECK(declNode->getIdentifier() == "x");
        CHECK(declNode->getVariableType() == "keyword 'int'");
        
        auto* numNode = require_node<IntegerExprAST>(declNode->getExpression());
        CHECK(numNode->getValue() == 42);
    }

    SECTION("Simple variable assignment statement: x = 11;") {
        Parser p{Lexer{"x = 11;"}.tokenize()};
        auto stmt = ParserTester::parseStmt(p);
        
        auto* assignNode = require_node<AssignmentStmtAST>(stmt.get());
        CHECK(assignNode->getIdentifier() == "x");
        
        auto* numNode = require_node<IntegerExprAST>(assignNode->getExpression());
        CHECK(numNode->getValue() == 11);
    }


    SECTION("Simple Print statement: print(42)") {
        Parser p{Lexer{"print(42);"}.tokenize()};
        auto stmt = ParserTester::parseStmt(p);
        
        auto* printNode = require_node<PrintStmtAST>(stmt.get());
        
        auto* numNode = require_node<IntegerExprAST>(printNode->getExpression());
        CHECK(numNode->getValue() == 42);
    }

    SECTION("Simple if statement: if(x > 3){ }") {
        Parser p{Lexer{"if(x > 3){ }"}.tokenize()}; 
        auto stmt = ParserTester::parseStmt(p);
        
        auto* ifNode = require_node<IfStmtAST>(stmt.get());
        
        auto* condNode = require_node<BinaryExprAST>(ifNode->getCondition());
        CHECK(condNode->getOp() == ">");
        
        auto* lhs = require_node<VariableExprAST>(condNode->getLHS());
        CHECK(lhs->getName() == "x");
    }

    SECTION("Simple if else statement: if(x > 3){ }else{ }") {
        Parser p{Lexer{"if(x > 3){ }else{ }"}.tokenize()};
        auto stmt = ParserTester::parseStmt(p);
        
        auto* ifNode = require_node<IfStmtAST>(stmt.get());
        
        auto* condNode = require_node<BinaryExprAST>(ifNode->getCondition());
        CHECK(condNode->getOp() == ">");
        
        auto* lhs = require_node<VariableExprAST>(condNode->getLHS());
        CHECK(lhs->getName() == "x");


    }

    SECTION("Simple while statement: while(x >= 0){ }") {
        Parser p{Lexer{"while(x >= 0){ }"}.tokenize()};
        auto stmt = ParserTester::parseStmt(p);
        
        auto* whileNode = require_node<WhileStmtAST>(stmt.get());
        
        auto* condNode = require_node<BinaryExprAST>(whileNode->getCondition());
        CHECK(condNode->getOp() == ">=");
    }

    SECTION("Simple return statement: return x * 2;") {
        Parser p{Lexer{"return x * 2;"}.tokenize()}; 
        auto stmt = ParserTester::parseStmt(p);
        
        auto* returnNode = require_node<ReturnStmtAST>(stmt.get());
        
        auto* binExpr = require_node<BinaryExprAST>(returnNode->getExpression());
        CHECK(binExpr->getOp() == "*");
    }   

}

TEST_CASE("Parser handles Function Calls", "[parser]")
{

    SECTION("Simple Call: function(42)") {
        Parser p{Lexer{"function(42);"}.tokenize()};
        auto expr = ParserTester::parseExpr(p);
        
        auto* call = require_node<CallExprAST>(expr.get());
        CHECK(call->getCallee() == "function");
        REQUIRE(call->getArgs().size() == 1);
        
        auto* num = require_node<IntegerExprAST>(call->getArgs()[0].get());
        CHECK(num->getValue() == 42);
    }

    //#7 issue Bug - expected ; after function(42)
    SECTION("Nested funciton call in statement: print(function(42))") {
        Parser p{Lexer{"print(function(42));"}.tokenize()};
        auto stmt = ParserTester::parseStmt(p);
        
        auto* printNode = require_node<PrintStmtAST>(stmt.get());
        
        auto* call = require_node<CallExprAST>(printNode->getExpression());
        CHECK(call->getCallee() == "function");
        REQUIRE(call->getArgs().size() == 1);
        
        auto* num = require_node<IntegerExprAST>(call->getArgs()[0].get());
        CHECK(num->getValue() == 42);
    }

}



struct TestCase {
    std::string input;          
    std::string expected_match; 
};

TEST_CASE("Parser handles expressions", "[parser]")
{
std::vector<TestCase> cases = {
        //Basic Arithmetic
        { "10 + 20", "(+ 10 20)" },
        { "50 - 5", "(- 50 5)" },
        { "4 * 2", "(* 4 2)" },
        { "10 / 2", "(/ 10 2)" },

        //Operator Precedence
        { "2 + 3 * 4", "(+ 2 (* 3 4))" },
        { "2 * 3 + 4", "(+ (* 2 3) 4)" },

        //Parentheses
        { "(2 + 3) * 4", "(* (+ 2 3) 4)" },

        //Comparison Operators
        { "x > 10", "(> x 10)" },
        { "y < 5", "(< y 5)" },
        { "a == b", "(== a b)" },
        { "a != b", "(!= a b)" },

        //Logical/Complex Nesting
        { "x > 0 + 5", "(> x (+ 0 5))" }, 
        { "10 + 20 * 30 / 40", "(+ 10 (/ (* 20 30) 40))" } 
    };

    for (const auto& c : cases) {
        CAPTURE(c.input);
        Lexer lexer{c.input};
        Parser parser{lexer.tokenize()};

        auto expr = ParserTester::parseExpr(parser);
        
        REQUIRE(expr != nullptr);
        CHECK(expr->toString().find(c.expected_match) != std::string::npos);
    }
}

