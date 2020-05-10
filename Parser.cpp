#include <sstream>
#include "Parser.hpp"
#include "AST/ProgramASTNode.h"
#include "AST/IntNode.h"
#include "AST/ConstNode.h"
#include "AST/CompoundNode.h"
#include "AST/VarNode.h"

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

    if(lexer->getToken().getKind() != Kind::tok_div)
        return logError("Missing ; after program declaration");

    std::vector<ASTNode*> declarations = {};

    if(!parseDeclaration(declarations))
        return false;
    declarations[0]->print();
    auto declarationsNode = new CompoundNode(declarations);

    auto programNode = ProgramASTNode(identifierToken.getValue(), declarationsNode, nullptr);

    programNode.print();

    return true;
}

bool Parser::parseDeclaration(std::vector<ASTNode*>& result) {
    Token nextToken = lexer->peekNextToken();
    ASTNode* currentNodeResult = nullptr;
    switch(nextToken.getKind()) {
        case Kind::tok_const:
            if(!parseConstDeclaration(currentNodeResult))
                return false;
            break;
        case Kind::tok_var:
            break;
        case Kind::tok_begin:
            return true;
        default:
            std::stringstream ss;
            ss << "Unexpected token " << nextToken.getValue();
            return logError(ss.str());
    }
    result.push_back(currentNodeResult);
    return parseDeclaration(result);
}

bool Parser::eat(Token token) {
    if(lexer->getToken().getKind() != token.getKind()) {
        std::stringstream ss;
        ss << "Unexpected token " << lexer->getToken().getValue() << " expected: " << token.getValue();
        return logError(ss.str());
    } else {
        return true;
    }
}

bool Parser::readIdentifier(Token& token) {
    Token identifier = lexer->getToken();
    if(identifier.getKind() != Kind::tok_identifier)
        return logError("Expected identifier");
    token = identifier;
    return true;
}

bool Parser::parseConstDeclaration(ASTNode*& result) {
    if(lexer->getToken().getKind() != Kind::tok_const)
        return logError("Expected const");

    Token identifier;
    if(!readIdentifier(identifier))
        return false;

    if(lexer->getToken().getKind() != Kind::tok_init)
        return logError("Expected =");

    ASTNode* value = nullptr;
    if(!parseExpression(value))
        return false;
    result = new ConstNode(identifier.getValue(), value);

    if(lexer->getToken().getKind() != Kind::tok_div)
        return logError("Missing ;");

    return true;
}

bool Parser::parseVarDeclaration(ASTNode *&result) {
    eat(Token(Kind::tok_var, "var"));

    Token identifier;
    if(!readIdentifier(identifier))
        return false;
    eat(Token(Kind::tok_type, ":"));

    Token type;
    if(!readIdentifier(type))
        return false;

    result = new VarNode(identifier.getValue(), type.getValue());

    eat(Token(Kind::tok_div, ";"));
}

bool Parser::parseExpression(ASTNode*& result) {
    // TODO: Reset of expression
    return parseTerm(result);
}

bool Parser::parseTerm(ASTNode*& result) {
    // TODO: Rest of term
    return parseFactor(result);
}

bool Parser::parseFactor(ASTNode*& result) {
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_integer: {
            int value = std::atoi(lexer->getToken().getValue().c_str());
            result = new IntNode(value);
            return true;
        }
        default: {
            std::stringstream ss;
            ss << "Unexpected token " << lexer->getToken().getValue();
            return logError(ss.str());
        }
    }
}

bool Parser::parseMain(ASTNode*& result) {
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

Token Parser::getNextToken() {
    return lexer->getToken();
}
