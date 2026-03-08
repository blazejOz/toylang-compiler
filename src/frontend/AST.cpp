#include "AST.hpp"


/// Constructors
PrintStmtAST::PrintStmtAST(std::unique_ptr<AST> expression) 
    : expression_(std::move(expression)) {}

VarDeclarationStmtAST::VarDeclarationStmtAST(TokenType vt, const std::string& ident, std::unique_ptr<AST> expr)
    : varType_{vt}, identifier_{ident}, expression_{std::move(expr)} {}

VariableExprAST::VariableExprAST(const std::string& name) : name_{name} {}

BiniaryExprAST::BiniaryExprAST(TokenType op, std::unique_ptr<AST> l, std::unique_ptr<AST> r)
    : op_{op}, lhs_{std::move(l)}, rhs_{std::move(r)} {}

IntegerExprAST::IntegerExprAST(int val) : val_{val} {}



///Codegen

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

llvm::Value* VarDeclarationStmtAST::codegen(llvm::LLVMContext& context, 
                                            llvm::IRBuilder<>& builder, 
                                            llvm::Module& module) 
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, 0, true)); //TODO
}

llvm::Value* VariableExprAST::codegen(llvm::LLVMContext& context, 
                                      llvm::IRBuilder<>& builder, 
                                      llvm::Module& module) 
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, 0, true)); //TODO
}

llvm::Value* BiniaryExprAST::codegen(llvm::LLVMContext& context, 
                                     llvm::IRBuilder<>& builder, 
                                     llvm::Module& module) 
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, 0, true)); //TODO
}

llvm::Value* IntegerExprAST::codegen(llvm::LLVMContext& context, 
                                     llvm::IRBuilder<>& builder, 
                                     llvm::Module& module) 
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, val_, true));
}


/// toString

std::string PrintStmtAST::toString()
{
    return "(print " + expression_->toString() + ")";
}

std::string VarDeclarationStmtAST::toString()
{
    return "(decl " + identifier_ + " " + expression_->toString() + ")";
}

std::string VariableExprAST::toString()
{
    return name_;
}

std::string BiniaryExprAST::toString()
{
    return "(" + Token::typeToString(op_) + " " + lhs_->toString() + " " + rhs_->toString() + ")";
}

std::string IntegerExprAST::toString()
{
    return std::to_string(val_);
}