#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>


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

