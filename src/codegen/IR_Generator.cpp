#include "IR_Generator.hpp"

void IRGenerator::generate(const std::vector<std::unique_ptr<AST>>& nodes) {
    // loop parser nodes
    for (const auto& node : nodes) {
        node->codegen(context, builder, *module, namedValues);
    }
}





//IR CODEGEN FOR AST NODES

///Codegen for Containers (functions / codeblocs)
llvm::Value* FunctionAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    //create the function type
    std::vector<llvm::Type*> ArgTypes(args_.size(), builder.getInt32Ty());
    llvm::Type* ReturnType = (returnType_ == TokenType::VOID_KEYWORD) ? builder.getVoidTy() : builder.getInt32Ty();
    llvm::FunctionType* FuntionType = llvm::FunctionType::get(ReturnType, ArgTypes, false);
    
    // create function object
    llvm::Function* function = llvm::Function::Create(FuntionType, llvm::Function::ExternalLinkage, name_, &module);
       
    // map arguments (clear the old, add new ones)
    namedValues.clear();
    unsigned idx = 0;
    for (auto& arg : function->args()) {
        std::string argName = args_[idx++].second;
        arg.setName(argName);
        namedValues[argName] = &arg;
    }

    // create entry block
    llvm::BasicBlock* BasicBlock = llvm::BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(BasicBlock);

    // generate body (BlockAST)
    if (body_) {
        body_->codegen(context, builder, module, namedValues);
    }

    // check Return and add one if missing
    if (!BasicBlock->getTerminator()) {
        if (returnType_ == TokenType::VOID_KEYWORD)
            builder.CreateRetVoid();
        else
            builder.CreateRet(builder.getInt32(0));
    }

    return function;
}

llvm::Value* BlockAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{

    return nullptr;
}

///Codegen for Statments

llvm::Value* VarDeclarationStmtAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    //Calculate expression (eg. 2+2*2)
    llvm::Value* initVal = expression_->codegen(context, builder, module, namedValues);
    //alloca instruction - reserve 32bits on the stack for int
    llvm::AllocaInst* alloca = builder.CreateAlloca(builder.getInt32Ty(), nullptr, identifier_);
    // Store Initval into alloca(allocated stack memory)
    builder.CreateStore(initVal, alloca);
    //Save the memory addres of variable in symbol table
    namedValues[identifier_] = alloca;

    return alloca;
}

llvm::Value* AssignmentStmtAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    //TODO !!!!
    return nullptr;
}


llvm::Value* PrintStmtAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    //Generete value to be printed
    llvm::Value* val = expression_->codegen(context, builder, module, namedValues);

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

llvm::Value* IfStmtAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    //TODO !!!!
    return nullptr;
}

llvm::Value* WhileStmtAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    //TODO !!!!
    return nullptr;
}

llvm::Value* ReturnStmtAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    //TODO !!!!
    return nullptr;
}

/// Codegen for expressions

llvm::Value* BinaryExprAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    llvm::Value* L = lhs_->codegen(context, builder, module, namedValues);
    llvm::Value* R = rhs_->codegen(context, builder, module, namedValues);

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

llvm::Value* VariableExprAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    llvm::Value *V = namedValues[name_];
    if (!V)
        LogErrorV("Unknown variable name");
    return V;
}

llvm::Value* IntegerExprAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, val_, true));
}

llvm::Value* CallExprAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    //TODO !!!!
    return nullptr;
}