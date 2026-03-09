#include "AST.hpp"


/// Constructors
PrintStmtAST::PrintStmtAST(std::unique_ptr<AST> expression) 
    : expression_(std::move(expression)) {}

VarDeclarationStmtAST::VarDeclarationStmtAST(TokenType vt, const std::string& ident, std::unique_ptr<AST> expr)
    : varType_{vt}, identifier_{ident}, expression_{std::move(expr)} {}

VariableExprAST::VariableExprAST(const std::string& name) : name_{name} {}

BinaryExprAST::BinaryExprAST(TokenType op, std::unique_ptr<AST> l, std::unique_ptr<AST> r)
    : op_{op}, lhs_{std::move(l)}, rhs_{std::move(r)} {}

IntegerExprAST::IntegerExprAST(int val) : val_{val} {}



///Codegen

llvm::Value* PrintStmtAST::codegen(llvm::LLVMContext& context, 
                            llvm::IRBuilder<>& builder, 
                            llvm::Module& module,
                            SymbolTable& symbolTable)
{
    //Generete value to be printed
    llvm::Value* val = expression_->codegen(context, builder, module, symbolTable);

    // Get or create format string "%d\n"
    llvm::Value* formatStr = module.getNamedGlobal("printf_fmt");
    if (!formatStr) {
        formatStr = builder.CreateGlobalString("%d\n", "printf_fmt");
    }

    //Get or create printf declaration
    llvm::Function* printfFunc = module.getFunction("printf");
    if (!printfFunc) {
        llvm::FunctionType* printfType = llvm::FunctionType::get(
            builder.getInt32Ty(), { builder.getPtrTy() }, true);
        printfFunc = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", &module);
    }

    return builder.CreateCall(printfFunc, { formatStr, val }, "printf_call");

}

llvm::Value* VarDeclarationStmtAST::codegen(llvm::LLVMContext& context, 
                                            llvm::IRBuilder<>& builder, 
                                            llvm::Module& module,
                                            SymbolTable& symbolTable) 
{
    //Calculate expression (eg. 2+2*2)
    llvm::Value* initVal = expression_->codegen(context, builder, module, symbolTable);
    //alloca instruction - reserve 32bits on the stack for int
    llvm::AllocaInst* alloca = builder.CreateAlloca(builder.getInt32Ty(), nullptr, identifier_);
    // Store Initval into alloca(allocated stack memory)
    builder.CreateStore(initVal, alloca);
    //Save the memory addres of variable in symbol table
    symbolTable[identifier_] = alloca;

    return alloca;
}

llvm::Value* VariableExprAST::codegen(llvm::LLVMContext& context, 
                                    llvm::IRBuilder<>& builder, 
                                    llvm::Module& module,
                                    SymbolTable& symbolTable) 
{
    //find variables memory in the stack
    llvm::AllocaInst* alloca = symbolTable[name_];
    //load memory adress and extract number inside
    return builder.CreateLoad(alloca->getAllocatedType(), alloca, name_.c_str());
}

llvm::Value* BinaryExprAST::codegen(llvm::LLVMContext& context, 
                                    llvm::IRBuilder<>& builder, 
                                    llvm::Module& module,
                                    SymbolTable& symbolTable) 
{
    llvm::Value* L = lhs_->codegen(context, builder, module, symbolTable);
    llvm::Value* R = rhs_->codegen(context, builder, module, symbolTable);

    switch (op_) {
        case TokenType::ADD:
            return builder.CreateAdd(L, R, "addtmp");
        case TokenType::SUB:
            return builder.CreateSub(L, R, "subtmp");
        case TokenType::MUL:
            return builder.CreateMul(L, R, "multmp");
        case TokenType::DIV:
            // SDiv - Signed Division
            return builder.CreateSDiv(L, R, "divtmp");
        default:
            std::cerr << "Unknown binary operator" << std::endl;
            return nullptr;
    }
}

llvm::Value* IntegerExprAST::codegen(llvm::LLVMContext& context, 
                                        llvm::IRBuilder<>& builder, 
                                        llvm::Module& module,
                                        SymbolTable& symbolTable) 
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

std::string BinaryExprAST::toString()
{
    return "(" + Token::typeToString(op_) + " " + lhs_->toString() + " " + rhs_->toString() + ")";
}

std::string IntegerExprAST::toString()
{
    return std::to_string(val_);
}