#include "JITengine.hpp"

JITengine::JITengine() {
    // init target machine CPU drivers 
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
}

void JITengine::runModule(std::unique_ptr<llvm::Module> module) {
    std::string error;

    //build engine
    engine_.reset(llvm::EngineBuilder(std::move(module))
        .setErrorStr(&error)
        .setEngineKind(llvm::EngineKind::JIT)
        .create());

    if (!engine_) {
        std::cerr << "JIT Error: " << error << std::endl;
        return;
    }

    // find main in 
    uint64_t addr = engine_->getFunctionAddress("main");

    if (addr == 0) {
        std::cerr << "JIT Error: Could not find main function address." << std::endl;
        return;
    }
    //i dont know... it wokrs...
    int (*compiledMain)() = (int (*)())addr;
    
    std::cout << "### Starting JIT" << std::endl;
    compiledMain(); 
    std::cout << "### JIT Finished" << std::endl;
}