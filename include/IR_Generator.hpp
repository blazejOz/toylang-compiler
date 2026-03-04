#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "AST.hpp"

class IRGenerator {
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;

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
    llvm::Module* getModule() { return module.get(); }
};