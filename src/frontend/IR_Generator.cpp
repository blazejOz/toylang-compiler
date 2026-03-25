#include "IR_Generator.hpp"

void IRGenerator::generate(const std::vector<std::unique_ptr<AST>>& nodes) 
{
    // loop parser nodes
    for (const auto& node : nodes) {
        node->codegen(context, builder, *module, namedValues);
    }
}

std::string IRGenerator::getIRString() const
{
    std::string ir;
    llvm::raw_string_ostream os(ir);
    module->print(os, nullptr);
    return ir;
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
    if (!builder.GetInsertBlock()->getTerminator()) {
        if (returnType_ == TokenType::VOID_KEYWORD)
            builder.CreateRetVoid();
        else
            builder.CreateRet(builder.getInt32(0));
    }

    return function;
}

llvm::Value* BlockAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    llvm::Value* lastValue = nullptr;
    for(auto& stmt: statements_){
        if(stmt) lastValue = stmt->codegen(context, builder, module, namedValues);
    }
    return lastValue;
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
    llvm::Value* variable = namedValues[identifier_];

    if(!variable){ LogErrorV("Assignment to undeclared variable"); }

    llvm::Value* newVal = expression_->codegen(context, builder, module, namedValues);
    if (!newVal) { LogErrorV("Assigning new value"); }

    builder.CreateStore(newVal, variable);
    return newVal;
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
    //evaluete condition
    llvm::Value* condValue = condition_->codegen(context, builder, module, namedValues);
    if(!condValue) LogErrorV("conditional expression");

    //create basic block for then / else / 
    llvm::Function* theFunction = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock* thenBB  = llvm::BasicBlock::Create(context, "then", theFunction);
    llvm::BasicBlock* elseBB  = llvm::BasicBlock::Create(context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "ifcont");


    builder.CreateCondBr(condValue, thenBB, elseBB);

    builder.SetInsertPoint(thenBB);
    thenBlock_->codegen(context, builder, module, namedValues);
    
    // block must end with a terminator(jump)
    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(mergeBB);
    }

    // add else value 
    theFunction->insert(theFunction->end(), elseBB);
    builder.SetInsertPoint(elseBB);
    
    if (elseBlock_) {
        elseBlock_->codegen(context, builder, module, namedValues);
    }

    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(mergeBB);
    }

    // Merge/Continue
    theFunction->insert(theFunction->end(), mergeBB);
    builder.SetInsertPoint(mergeBB);

    return llvm::Constant::getNullValue(builder.getInt32Ty());
}

llvm::Value* WhileStmtAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
   llvm::Function* theFunction = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* condBB  = llvm::BasicBlock::Create(context, "while_cond", theFunction);
    llvm::BasicBlock* bodyBB  = llvm::BasicBlock::Create(context, "while_body");
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(context, "after_while");

    builder.CreateBr(condBB);

    //setup condition Block
    builder.SetInsertPoint(condBB);
    llvm::Value* condV = condition_->codegen(context, builder, module, namedValues);
    if (!condV) LogErrorV("while loop condition expression");

    // If true -> body / If false -> after
    builder.CreateCondBr(condV, bodyBB, afterBB);

    //setup body Block
    theFunction->insert(theFunction->end(), bodyBB); //add body to function
    builder.SetInsertPoint(bodyBB);
    
    if (body_) {
        body_->codegen(context, builder, module, namedValues);
    }


    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(condBB);
    }

    // exit Block
    theFunction->insert(theFunction->end(), afterBB);
    builder.SetInsertPoint(afterBB);

    //return dummy value(null)
    return llvm::Constant::getNullValue(builder.getInt32Ty());
}

llvm::Value* ReturnStmtAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    llvm::Value* val = expression_->codegen(context, builder, module, namedValues);
    if (!val) LogErrorV("return expression");

    return builder.CreateRet(val);
}

/// Codegen for expressions

llvm::Value* BinaryExprAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    llvm::Value* L = lhs_->codegen(context, builder, module, namedValues);
    llvm::Value* R = rhs_->codegen(context, builder, module, namedValues);

    switch (op_) {
        //ARITHEMETIC
        case TokenType::ADD:
            return builder.CreateAdd(L, R, "addtmp");
        case TokenType::SUB:
            return builder.CreateSub(L, R, "subtmp");
        case TokenType::MUL:
            return builder.CreateMul(L, R, "multmp");
        case TokenType::DIV:
            return builder.CreateSDiv(L, R, "divtmp"); // SDiv - Signed Division

        //COMPARISION
        case TokenType::EQ:
            return builder.CreateICmpEQ(L, R, "eqtmp");
        case TokenType::NOT_EQ:
            return builder.CreateICmpNE(L, R, "netmp");
        case TokenType::GT:
            return builder.CreateICmpSGT(L, R, "gttmp"); // SGT = Signed Greater Than
        case TokenType::LT:
            return builder.CreateICmpSLT(L, R, "lttmp"); // SLT = Signed Less Than
        case TokenType::GE:
            return builder.CreateICmpSGE(L, R, "getmp"); // SGE = Signed Greater Equal
        case TokenType::LE:
            return builder.CreateICmpSLE(L, R, "letmp"); // SLE = Signed Less Equal
        default:
            LogErrorV("Unknown Binary Operation");
            return nullptr;
    }
}

llvm::Value* VariableExprAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    llvm::Value *value = namedValues[name_];
    if (!value)
        LogErrorV("Unknown variable name");
    
    
    if (llvm::isa<llvm::AllocaInst>(value)) {
        return builder.CreateLoad(builder.getInt32Ty(), value, name_.c_str());
    }
    return value;
}

llvm::Value* IntegerExprAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, value_, true));
}

llvm::Value* CallExprAST::codegen(Context context, IRBuild builder, Mod module, NamedVals namedValues)
{
    // look up function name in the global module table
    llvm::Function* calleeFunc = module.getFunction(callee_);
    
    if (!calleeFunc) LogErrorV(" Unknown function reference");
    if (calleeFunc->arg_size() != args_.size()) LogErrorV("Wrong argument count");

    //gen ir for each argument
    std::vector<llvm::Value*> argsV;
    for (auto& arg : args_) {
        argsV.push_back(arg->codegen(context, builder, module, namedValues));
        
        if (!argsV.back()) LogErrorV("Arg Genereation");
    }
    //llvm call instruction
    bool isVoid = calleeFunc->getReturnType()->isVoidTy();
    return builder.CreateCall(calleeFunc, argsV, isVoid ? "" : "calltmp");
}
