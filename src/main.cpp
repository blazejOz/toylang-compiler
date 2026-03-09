#include "Lexer.hpp"
#include "Parser.hpp"
#include "IR_Generator.hpp"
#include "JITengine.hpp"

int main() {
    // 1. Lexing
    Lexer lexer("int x = 2 + 2 * 2; print(x); print(4 + 2 * 7 + 10);");
    auto tokens = lexer.tokenize();

    // 2. Parsing
    Parser parser(tokens);
    auto asts = parser.parse();

    for (const auto& ast : asts) {
        std::cout << "Parsed Statement: " << ast->toString() << std::endl;
    }

    // 3. IR Generation
    IRGenerator codegen;
    codegen.generate(asts);
    codegen.print();

    

    JITengine engine;
    engine.runModule(codegen.moveModule());
    

    return 0;
}