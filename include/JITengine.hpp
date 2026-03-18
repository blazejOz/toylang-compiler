#pragma once
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/Module.h>
#include <iostream>

/** @brief Simple Just-In-Time compiler wrapper using LLVM JITengine.
 *  This class handles the translation of LLVM IR from a Module into 
 *  machine code in memory and executes the 'main' entry point. */
class JITengine
{
private:
    std::unique_ptr<llvm::ExecutionEngine> engine_;

public:
    JITengine();

    /** @brief Takes ownership of an LLVM module and executes its 'main' function. */
    void runModule(std::unique_ptr<llvm::Module> module);
};