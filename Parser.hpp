#ifndef PJPPROJECT_PARSER_HPP
#define PJPPROJECT_PARSER_HPP

#include "Lexer.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "AST/ASTNode.h"
#include <vector>

using namespace llvm;

class Parser {
public:
    explicit Parser(std::unique_ptr<Lexer>& lexer);
    ~Parser() = default;
    Parser(const Parser &parser);

    bool Parse();             // parse
    const Module& Generate(); // generate

private:
    Token getNextToken();

    bool logError(const std::string& reason);

    bool parseProgram();
    bool parseDeclaration(std::vector<ASTNode*>& result);
    bool parseConstDeclaration(ASTNode*& result);
    bool parseVarDeclaration(ASTNode*& result);
    bool parseBlock(std::vector<ASTNode *> &result);
    bool parseInstruction(std::vector<ASTNode *>&result);
    bool parseAssign(ASTNode *&result, Token identifier);
    bool parseProcedure(ASTNode *&result, Token identifier);
    bool parseExpression(ASTNode*& result);
    bool parseRestExpression(ASTNode* previousExpression, ASTNode*& result);
    bool parseRestTerm(ASTNode* previousTerm, ASTNode*& result);
    bool parseTerm(ASTNode*& result);
    bool parseFactor(ASTNode*& result);
    bool eat(Token token);
    bool readIdentifier(Token& token);
    
    Lexer* lexer;            // lexer is used to read tokens
    
    LLVMContext MilaContext;   // llvm context
    IRBuilder<> MilaBuilder;   // llvm builder
    Module MilaModule;         // llvm module
};

#endif //PJPPROJECT_PARSER_HPP
