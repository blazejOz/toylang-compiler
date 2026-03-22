#pragma once

#include <llvm/IR/Module.h> 
#include <memory>           
#include <string>    


class Backend
{
private:

public:
    void emitObjectFile(std::unique_ptr<llvm::Module> module, std::string filename);

};