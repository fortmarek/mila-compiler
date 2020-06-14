#ifndef PJPPROJECT_PARSER_HPP
#define PJPPROJECT_PARSER_HPP

#include "Lexer.hpp"

#include "AST/ASTNode.h"
#include <vector>

using namespace llvm;

class Parser {
public:
    explicit Parser(std::unique_ptr<Lexer>& lexer);
    ~Parser() = default;

    bool Parse();             // parse
    ASTNode* getProgram();

private:
    Token getNextToken();

    bool logError(const std::string& reason);

    bool parseProgram();
    bool parseDeclaration(std::vector<ASTNode*>& result);
    bool parseConstDeclaration(ASTNode*& result);
    bool parseVarDeclaration(ASTNode*& result);
    bool parseRestVarDeclaration(std::vector<ASTNode*>& result);
    bool parseBlock(std::vector<ASTNode *> &result);
    bool parseWhileBlock(ASTNode* & result);
    bool parseForBlock(ASTNode*& result);
    bool parseIfBlock(ASTNode*& result);
    bool parseCondition(ASTNode*& result);
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

    ASTNode* program;
    
    Lexer* lexer;            // lexer is used to read tokens
};

#endif //PJPPROJECT_PARSER_HPP
