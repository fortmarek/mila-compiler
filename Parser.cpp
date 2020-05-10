#include <sstream>
#include "Parser.hpp"
#include "AST/ProgramASTNode.h"
#include "AST/IntNode.h"
#include "AST/ConstNode.h"
#include "AST/CompoundNode.h"
#include "AST/VarNode.h"
#include "AST/ProcedureNode.h"
#include "AST/AssignNode.h"
#include "AST/IdentifierNode.h"

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
    auto declarationsNode = new CompoundNode(declarations);

    std::vector<ASTNode*> instructions;
    if(!parseBlock(instructions))
        return false;
    auto instructionsNode = new CompoundNode(instructions);

    auto programNode = ProgramASTNode(identifierToken.getValue(), declarationsNode, instructionsNode);

    programNode.print();

    eat(Token(Kind::tok_dot, "."));

    return true;
}

bool Parser::parseDeclaration(std::vector<ASTNode*>& result) {
    ASTNode* currentNodeResult = nullptr;
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_const:
            if(!parseConstDeclaration(currentNodeResult))
                return false;
            break;
        case Kind::tok_var:
            if(!parseVarDeclaration(currentNodeResult))
                return false;
            break;
        case Kind::tok_begin:
            return true;
        default:
            std::stringstream ss;
            ss << "Unexpected token " << lexer->peekNextToken().getValue();
            return logError(ss.str());
    }
    result.push_back(currentNodeResult);
    return parseDeclaration(result);
}

bool Parser::parseBlock(std::vector<ASTNode *> &result) {
    if(!eat(Token(Kind::tok_begin, "begin")))
        return false;
    if(!parseInstruction(result))
        return false;
    return eat(Token(Kind::tok_end, "end"));
}

bool Parser::parseInstruction(std::vector<ASTNode *> &result) {
    ASTNode* currentNodeResult = nullptr;
    Token identifier;
    if(!readIdentifier(identifier))
        return false;
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_left_paren:
            if(!parseProcedure(currentNodeResult, identifier))
                return false;
            break;
        case Kind::tok_assign:
            if(!parseAssign(currentNodeResult, identifier))
                return false;
        case Kind::tok_end:
            return true;
        default:
            std::stringstream ss;
            ss << "Unexpected token " << lexer->peekNextToken().getValue() << " " << lexer->peekNextToken().getKind();
            return logError(ss.str());
    }

    result.push_back(currentNodeResult);
    return parseInstruction(result);
}

bool Parser::parseAssign(ASTNode *&result, Token identifier) {
    if(!eat(Token(Kind::tok_assign, ":=")))
        return false;

    ASTNode* valueNode = nullptr;
    if(!parseExpression(valueNode))
        return false;

    result = new AssignNode(identifier.getValue(), valueNode);

    return eat(Token(Kind::tok_div, ";"));
}

bool Parser::parseProcedure(ASTNode *&result, Token identifier) {
    if(!eat(Token(Kind::tok_left_paren, "(")))
        return false;

    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_right_paren:
            result = new ProcedureNode(lexer->getToken().getValue());
            return eat(Token(Kind::tok_right_paren, ")")) && eat(Token(Kind::tok_div, ";"));
        default:
            // TODO: Multiple parameters
            ASTNode* parameter = nullptr;
            if(!parseExpression(parameter))
                return false;
            result = new ProcedureNode(identifier.getValue(), {parameter});
            return eat(Token(Kind::tok_right_paren, ")")) && eat(Token(Kind::tok_div, ";"));
    }
}

bool Parser::eat(Token token) {
    if(lexer->getToken().getKind() != token.getKind()) {
        std::stringstream ss;
        ss << "Unexpected token " << lexer->getToken().getValue() << ", expected: " << token.getValue();
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
    if(!eat(Token(Kind::tok_var, "var")))
        return false;

    Token identifier;
    if(!readIdentifier(identifier))
        return false;
    if(!eat(Token(Kind::tok_type, ":")))
        return false;

    Token type;
    if(!readIdentifier(type))
        return false;

    result = new VarNode(identifier.getValue(), type.getValue());

    return eat(Token(Kind::tok_div, ";"));
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
        case Kind::tok_identifier:
            result = new IdentifierNode(lexer->getToken().getValue());
            return true;
        default: {
            std::stringstream ss;
            ss << "Unexpected token " << lexer->getToken().getValue();
            return logError(ss.str());
        }
    }
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
