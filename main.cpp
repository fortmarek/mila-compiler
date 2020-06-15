#include <fstream>
#include <iostream>
#include "Parser.hpp"
#include "ASTWalker.h"

//Use tutorials in: https://llvm.org/docs/tutorial/

int main (int argc, char *argv[]) {
    std::ifstream in("samples/playground.mila");
    std::cin.rdbuf(in.rdbuf());

    std::unique_ptr<Lexer> Lex = std::make_unique<Lexer>();
    auto parser = Parser(Lex);

    if (!parser.Parse()) {
        return 1;
    }

    auto walker = new ASTWalker();

    walker->generate(parser.getProgram()).print(outs(), nullptr);

    return 0;
}
