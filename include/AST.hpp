#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include "Token.hpp"

//Type Aliases
using SymbolTable = std::map<std::string, llvm::AllocaInst*>;
using Context     = llvm::LLVMContext&;
using IRBuild     = llvm::IRBuilder<>&;
using Mod         = llvm::Module&;

/** @brief Base class for all Abstract Syntax Tree nodes.
 *  All specialized nodes (Expressions, Statements, Declarations) inherit from this.*/
class AST {
public:
    virtual ~AST() = default;

    /** @brief Generates LLVM IR for this specific node.
     *  @return llvm::Value* representing the result of the instruction or nullptr.*/
    virtual llvm::Value* codegen(Context context, IRBuild builder, Mod module, 
                                 SymbolTable& symbolTable) = 0;

    /** @brief Returns a string representation of the node (Lisp-style) for debugging. */
    virtual std::string toString() = 0;
};
class BlockAST;

//Container Nodes

/** @brief Represents a function definition with arguments and a body. */
class FunctionAST : public AST {
private:
    std::string name_;
    std::vector<std::pair<TokenType, std::string>> args_; // {Type, Name}
    TokenType returnType_;
    std::unique_ptr<BlockAST> body_;
public:
    FunctionAST(std::string name, std::vector<std::pair<TokenType, std::string>> args, 
                TokenType retType, std::unique_ptr<BlockAST> body)
        : name_(std::move(name)), args_(std::move(args)), returnType_(retType), body_(std::move(body)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return "(function " + name_  + ")";  }
};


/** @brief Represents a block of code enclosed in braces { ... } */
class BlockAST : public AST {
private:
    std::vector<std::unique_ptr<AST>> statements_;
public:
    BlockAST(std::vector<std::unique_ptr<AST>> statements)
        : statements_(std::move(statements)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return "{ " + statements_[0]->toString() + "}";  }
};


//Statement Nodes


/** @brief Represents a variable declaration (e.g., int x = 5). */
class VarDeclarationStmtAST : public AST {
private:
    TokenType varType_;
    std::string identifier_;
    std::unique_ptr<AST> expression_;
public:
    VarDeclarationStmtAST(TokenType vt, std::string id, std::unique_ptr<AST> expr)
        : varType_(vt), identifier_(std::move(id)), expression_(std::move(expr)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return "(decl " + identifier_ + " " + expression_->toString() + ")"; }
};

/** @brief Represents an assignment to an existing variable (e.g., x = 10). */
class AssignmentStmtAST : public AST {
private:
    std::string identifier_;
    std::unique_ptr<AST> expression_;
public:
    AssignmentStmtAST(std::string id, std::unique_ptr<AST> expr)
        : identifier_(id), expression_(std::move(expr)) {}
        
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return "(assign " + identifier_ + " " + expression_->toString() + ")"; }
};

/** @brief Represents a statement that prints an expression. */
class PrintStmtAST : public AST {
private:
    std::unique_ptr<AST> expression_;
public:
    PrintStmtAST(std::unique_ptr<AST> expression)
        : expression_(std::move(expression)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override {     return "(print " + expression_->toString() + ")";  }
};

/** @brief Represents an if-else conditional branch. */
class IfStmtAST : public AST {
private:
    std::unique_ptr<AST> condition_;
    std::unique_ptr<BlockAST> thenBlock_;
    std::unique_ptr<BlockAST> elseBlock_; // Can be nullptr
public:
    IfStmtAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> thenB, std::unique_ptr<BlockAST> elseB)
        : condition_(std::move(cond)), thenBlock_(std::move(thenB)), elseBlock_(std::move(elseB)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return "(if " + condition_->toString() + ")"; }
};

/** @brief Represents a while loop: while (condition) { body } */
class WhileStmtAST : public AST {
private:
    std::unique_ptr<AST> condition_;
    std::unique_ptr<BlockAST> body_;
public:
    WhileStmtAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> body)
        : condition_(std::move(cond)), body_(std::move(body)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return "(while " + condition_->toString() + ")"; }
};

/** @brief Represents a return statement. */
class ReturnStmtAST : public AST {
private:
    std::unique_ptr<AST> expression_;
public:
    ReturnStmtAST(std::unique_ptr<AST> expression)
        : expression_(std::move(expression)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return "(return " + expression_->toString() + ")";}
};

//Expression Nodes

/** @brief Represents a binary operation (e.g., +, -, *, /, <, >, !=, ==). */
class BinaryExprAST : public AST {
private:
    TokenType op_;
    std::unique_ptr<AST> lhs_, rhs_;
public:
    BinaryExprAST(TokenType op, std::unique_ptr<AST> l, std::unique_ptr<AST> r)
        : op_(op), lhs_(std::move(l)), rhs_(std::move(r)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return "(" + Token::typeToString(op_) + " " + lhs_->toString() + " " + rhs_->toString() + ")"; }
};

/** @brief Represents a variable usage as an expression. */
class VariableExprAST : public AST {
private:
    std::string name_;
public:
    VariableExprAST(std::string name): name_(std::move(name)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return name_; }
};

/** @brief Represents a literal integer like '42'. */
class IntegerExprAST : public AST {
private:
    int val_;
public:
    IntegerExprAST(int val): val_(val) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override { return std::to_string(val_); }
};

/** @brief Represents a function call: identifier(args) */
class CallExprAST : public AST {
private:
    std::string callee_;
    std::vector<std::unique_ptr<AST>> args_;
public:
    CallExprAST(std::string callee, std::vector<std::unique_ptr<AST>> args)
        : callee_(callee), args_(std::move(args)) {}
    llvm::Value* codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable) override;
    std::string toString() override {  return "(" + callee_ +"()"; }
};