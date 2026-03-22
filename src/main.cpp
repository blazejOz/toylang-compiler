#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "IR_Generator.hpp"
#include "JITengine.hpp"
#include "Backend.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " <input_file> [-S | -o | -jit]" << std::endl;
            return 1;
        }

    std::string fileName = argv[1];
    std::string mode = (argc > 2) ? argv[2] : "-jit"; // Default to JIT if no flag

    //read file
    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string sourceCode = buffer.str();

    //compiler pipeline
    Lexer lexer(sourceCode);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto asts = parser.parse();

    IRGenerator codegen;
    codegen.generate(asts);

    //handle modes
    if (mode == "-S") {
        // Emit IR code to terminal
        codegen.print();
    } 
    else if (mode == "-jit") {
        // Run immediately
        std::string irCodes = codegen.getIRString();
        JITengine engine;
        engine.runFromIRString(irCodes);
    }else if (mode == "-o"){
        Backend backend;
        std::string objFile = fileName + ".o";
        backend.emitObjectFile(codegen.moveModule(), objFile);

        // Call the system linker (cc) to turn the .o into an executable
        std::string linkCmd = "cc " + objFile + " -o a.out";
        system(linkCmd.c_str());
        
        std::cout << "Successfully compiled " << fileName << " to a.out" << std::endl;
    }else {
        std::cerr << "Unknown flag: " << mode << std::endl;
        return 1;
    }

    return 0;

}