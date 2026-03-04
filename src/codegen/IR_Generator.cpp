#include "IR_Generator.hpp"

void IRGenerator::generate(const std::vector<std::unique_ptr<AST>>& nodes) {
    // setup main
    llvm::FunctionType* mainType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainType, 
                               llvm::Function::ExternalLinkage, "main", module.get());
    
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entry);

    // loop parser nodes
    for (const auto& node : nodes) {
        node->codegen(context, builder, *module);
    }

    // return 0
    builder.CreateRet(builder.getInt32(0));
}