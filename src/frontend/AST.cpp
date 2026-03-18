#include "AST.hpp"

///Codegen for Containers (functions / codeblocs)
llvm::Value* FunctionAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{
    //TODO !!!!
    return nullptr;
}

llvm::Value* BlockAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{

    return nullptr;
}

///Codegen for Statments

llvm::Value* VarDeclarationStmtAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
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

llvm::Value* AssignmentStmtAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{
    //TODO !!!!
    return nullptr;
}


llvm::Value* PrintStmtAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
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

llvm::Value* IfStmtAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{
    //TODO !!!!
    return nullptr;
}

llvm::Value* WhileStmtAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{
    //TODO !!!!
    return nullptr;
}

llvm::Value* ReturnStmtAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{
    //TODO !!!!
    return nullptr;
}

/// Codegen for expressions

llvm::Value* BinaryExprAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
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

llvm::Value* VariableExprAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{
    //find variables memory in the stack
    llvm::AllocaInst* alloca = symbolTable[name_];
    //load memory adress and extract number inside
    return builder.CreateLoad(alloca->getAllocatedType(), alloca, name_.c_str());
}

llvm::Value* IntegerExprAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, val_, true));
}

llvm::Value* CallExprAST::codegen(Context context, IRBuild builder, Mod module, SymbolTable& symbolTable)
{
    //TODO !!!!
    return nullptr;
}