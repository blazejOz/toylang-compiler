#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include "Token.hpp"


/**
 * @brief Base class for all expression nodes
 */
class AST
{
public:
    virtual ~AST() = default;
    virtual llvm::Value* codegen(llvm::LLVMContext& context, 
                                 llvm::IRBuilder<>& builder, 
                                 llvm::Module& module) = 0;
};

/**
 * @brief Represents a 'print' statement
 */
class PrintStmtAST: public AST
{
private:
    std::unique_ptr<AST> expression_;
public:
    PrintStmtAST(std::unique_ptr<AST> expression);
    llvm::Value* codegen(llvm::LLVMContext& context, 
                         llvm::IRBuilder<>& builder, 
                         llvm::Module& module) override;
};

/**
 * @brief Represesnts variable declaration eg. int x = 2 + 2 * 10;
 */
class VarDeclarationStmtAST: public AST
{
private:
    TokenType varType_; // int
    std::string identifier_;
    std::unique_ptr<AST> expression_;

public:
    VarDeclarationStmtAST(TokenType vt, const std::string&, std::unique_ptr<AST> expr);
    llvm::Value* codegen(llvm::LLVMContext& context, 
                         llvm::IRBuilder<>& builder, 
                         llvm::Module& module) override;
};

class VariableExprAST : public AST
{
private:
    std::string name_;
public:
    VariableExprAST(const std::string& name);
    llvm::Value* codegen(llvm::LLVMContext& context, 
                         llvm::IRBuilder<>& builder, 
                         llvm::Module& module) override;
};

/**
 * @brief Binary expression ( + * + /)
 */
class BiniaryExprAST : public AST
{
private:
    TokenType op_;
    std::unique_ptr<AST> lhs_; //LEFT HAND SIDE
    std::unique_ptr<AST> rhs_; //RIGHT HAND SIDE
public:
    BiniaryExprAST(TokenType op, std::unique_ptr<AST> l, std::unique_ptr<AST> r);
    llvm::Value* codegen(llvm::LLVMContext& context, 
                         llvm::IRBuilder<>& builder, 
                         llvm::Module& module) override;
};

/**
 * @brief Represents a literal integer like '1' or '42'
 */
class IntegerExprAST : public AST 
{
private:
    int val_;
public:
    IntegerExprAST(int val);
    llvm::Value* codegen(llvm::LLVMContext& context, 
                         llvm::IRBuilder<>& builder, 
                         llvm::Module& module) override;
};



