#include "Parser.hpp"
#include "AST/ProgramASTNode.h"

Parser::Parser(std::unique_ptr<Lexer>& lexer) : lexer(lexer.release()), MilaContext(), MilaBuilder(MilaContext), MilaModule("mila", MilaContext) {}

Parser::Parser(const Parser &parser) : MilaContext(), MilaBuilder(MilaContext), MilaModule("mila", MilaContext) {
    lexer = parser.lexer;
}

bool Parser::logError(const std::string &reason) {
    std::cerr << "Parsing failed with error: " << reason << std::endl;
    return false;
}

bool Parser::parseProgram() {
    Token token = getNextToken();

    if(token.getKind() != Kind::tok_program)
        return logError("Program must start with 'program name'");

    Token identifierToken = getNextToken();
    if(identifierToken.getKind() != Kind::tok_identifier)
        return logError("program must be followed by identifier");
    auto programNode = ProgramASTNode(identifierToken.getValue());

    programNode.print();

    return true;
}

bool Parser::Parse() {
    return parseProgram();
}

const Module& Parser::Generate() {

    // create writeln function
    {
      std::vector<Type*> Ints(1, Type::getInt32Ty(MilaContext));
      FunctionType * FT = FunctionType::get(Type::getInt32Ty(MilaContext), Ints, false);
      Function * F = Function::Create(FT, Function::ExternalLinkage, "writeln", MilaModule);
      for (auto & Arg : F->args())
          Arg.setName("x");
    }

    // create main function
    {
      FunctionType * FT = FunctionType::get(Type::getInt32Ty(MilaContext), false);
      Function * MainFunction = Function::Create(FT, Function::ExternalLinkage, "main", MilaModule);

      // block
      BasicBlock * BB = BasicBlock::Create(MilaContext, "entry", MainFunction);
      MilaBuilder.SetInsertPoint(BB);

      // call writeln with value from lexel
      MilaBuilder.CreateCall(MilaModule.getFunction("writeln"), {
        ConstantInt::get(MilaContext, APInt(32, lexer->numVal()))
      });

      // return 0
      MilaBuilder.CreateRet(ConstantInt::get(Type::getInt32Ty(MilaContext), 0));
    }

    return this->MilaModule;
}

/*
 * Simple token buffer.
 * CurTok is the current token the parser is looking at
 * getNextToken reads another token from the lexer and updates curTok with ts result
 * Every function in the parser will assume that CurTok is the cureent token that needs to be parsed
 */
Token Parser::getNextToken() {
    return lexer->gettok();
}
