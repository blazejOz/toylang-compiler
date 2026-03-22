#include "Backend.hpp"

// LLVM Target and Hardware
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/TargetSelect.h>    
#include <llvm/TargetParser/Host.h> 
#include <llvm/TargetParser/Triple.h>

// LLVM IR and File System
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>

#include <system_error>
#include <iostream>

void Backend::emitObjectFile(std::unique_ptr<llvm::Module> module, std::string filename) {
    //initialize native targets
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    // setup triple
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(llvm::Triple(targetTriple));

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!target) {
        llvm::errs() << error;
        return; 
    }

    //setup machine
    auto CPU = "generic";
    auto features = "";
    llvm::TargetOptions opt;
    auto targetMachine = target->createTargetMachine(
        targetTriple, CPU, features, opt, llvm::Reloc::PIC_);

    // configure Module
    module->setDataLayout(targetMachine->createDataLayout());

    // open file
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }

    //emit object code
    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        llvm::errs() << "TheTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*module);
    dest.flush();

    llvm::outs() << "Successfully wrote " << filename << "\n";
}