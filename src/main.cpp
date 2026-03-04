#include "Lexer.hpp"
#include "Parser.hpp"
#include "IR_Generator.hpp"

int main() {
    // 1. Lexing
    Lexer lexer("print(42);");
    auto tokens = lexer.tokenize();

    // 2. Parsing
    Parser parser(tokens);
    auto ast = parser.parse();

    // 3. IR Generation
    IRGenerator codegen;
    codegen.generate(ast);
    codegen.print();

    return 0;
}