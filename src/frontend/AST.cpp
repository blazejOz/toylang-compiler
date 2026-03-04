#include "AST.hpp"


// Constructors
IntegerExprAST::IntegerExprAST(int val) : val_{val} {}

PrintStmtAST::PrintStmtAST(std::unique_ptr<AST> expression) 
    : expression_(std::move(expression)) {}


// Integer Codegen
llvm::Value* IntegerExprAST::codegen(llvm::LLVMContext& context, 
                                     llvm::IRBuilder<>& builder, 
                                     llvm::Module& module) 
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, val_, true));
}

llvm::Value* PrintStmtAST::codegen( llvm::LLVMContext& context,
                                    llvm::IRBuilder<>& builder, 
                                    llvm::Module& module)
{
    llvm::Value* val = expression_->codegen(context, builder, module);

    auto formatStr = builder.CreateGlobalString("%d\n", "printf_fmt");

    llvm::Function* printfFunc = module.getFunction("printf");

    if (!printfFunc) {
        std::vector<llvm::Type*> args = { builder.getPtrTy() };
        llvm::FunctionType* printfType = llvm::FunctionType::get(
            builder.getInt32Ty(), // Return Type
            args,                 // Argument Types
            true                  // "isVarArg" 
        );
        
        printfFunc = llvm::Function::Create(
            printfType, 
            llvm::Function::ExternalLinkage, 
            "printf", 
            &module
        );
    }

    std::vector<llvm::Value*> printArgs = { formatStr, val };
    return builder.CreateCall(printfFunc, printArgs, "printf_call");
}
