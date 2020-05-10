#include "Parser.hpp"

//Use tutorials in: https://llvm.org/docs/tutorial/

int main (int argc, char *argv[]) {
    std::unique_ptr<Lexer> Lex = std::make_unique<Lexer>();
    auto parser = Parser(Lex);

    if (!parser.Parse()) {
        return 1;
    }

    parser.Generate().print(outs(), nullptr);

    return 0;
}
