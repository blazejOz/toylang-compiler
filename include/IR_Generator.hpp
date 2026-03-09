#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <map>
#include "AST.hpp"



class IRGenerator {
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::AllocaInst*> symbolTable_;

public:
    IRGenerator() 
        : builder(context), 
          module(std::make_unique<llvm::Module>("MyCompiler", context)) {}

    /**
     * @brief generete IR from AST trees
     */
    void generate(const std::vector<std::unique_ptr<AST>>& asts);
    
    /**
     * @brief Print IR code
     */
    void print() const { module->print(llvm::outs(), nullptr); }
    
    /**
     * @brief LLVM modlue getter
     * @return llvm::Module
    */
    std::unique_ptr<llvm::Module> moveModule() {
        return std::move(module);
    }
};