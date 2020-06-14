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
#include "AST/BinOpNode.h"
#include "AST/MainNode.h"
#include "AST/ForNode.h"
#include "AST/IfElseNode.h"

Parser::Parser(std::unique_ptr<Lexer>& lexer) : lexer(lexer.release()) {}

ASTNode* Parser::getProgram() {
    return program;
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

    if(lexer->getToken().getKind() != Kind::tok_divider)
        return logError("Missing ; after program declaration");

    std::vector<ASTNode*> declarations = {};

    if(!parseDeclaration(declarations))
        return false;
    auto declarationsNode = new CompoundNode({});

//    std::vector<ASTNode*> instructions;
    if(!parseBlock(declarations))
        return false;
    auto mainNode = new MainNode(declarations);

    auto programNode = new ProgramASTNode(identifierToken.getValue(), declarationsNode, mainNode);

//    programNode->print();

    if(!eat(Token(Kind::tok_dot, ".")))
        return false;

    program = programNode;

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
//    std::cout << "Parsing instruction" << std::endl;
    ASTNode* currentNodeResult = nullptr;
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_if:
            if(!parseIfBlock(currentNodeResult))
                return false;
            break;
        case Kind::tok_for:
            if(!parseForBlock(currentNodeResult))
                return false;
            break;
        default:
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
                    if(!eat(Token(Kind::tok_divider, ";")))
                        return false;
//            std::cout << "parsed assign" << std::endl;
                    break;
                case Kind::tok_end:
                    return true;
                default:
                    std::stringstream ss;
                    ss << "Unexpected token " << lexer->peekNextToken().getValue() << " " << lexer->peekNextToken().getKind();
                    return logError(ss.str());
            }
    }

    result.push_back(currentNodeResult);
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_end:
            return true;
        case Kind::tok_else:
            return true;
        default:
            return parseInstruction(result);
    }
}

bool Parser::parseForBlock(ASTNode *&result) {
    if(!eat(Token(Kind::tok_for, "for")))
        return false;

    Token identifier;
    if(!readIdentifier(identifier))
        return false;

    if(!eat(Token(Kind::tok_assign, ":=")))
        return false;

    ASTNode* startNode;
    if(!parseExpression(startNode))
        return false;

    if(!eat(Token(Kind::tok_to, "to")))
        return false;

    ASTNode* endNode;
    if(!parseExpression(endNode))
        return false;

    if(!eat(Token(Kind::tok_do, "do")))
        return false;

    if(!eat(Token(Kind::tok_begin, "begin")))
        return false;

    std::vector<ASTNode*> bodyInstructions = {};
    if(!parseInstruction(bodyInstructions))
        return false;

    if(!eat(Token(Kind::tok_end, "end")))
        return false;

    if(!eat(Token(Kind::tok_divider, ";")))
        return false;

    ASTNode* bodyNode = new CompoundNode(bodyInstructions);

    result = new ForNode(identifier.getValue(), startNode, endNode, bodyNode);

    return true;
}

bool Parser::parseIfBlock(ASTNode *&result) {
    if(!eat(Token(Kind::tok_if, "if")))
        return false;
    ASTNode* conditionNode;
    if(!parseCondition(conditionNode))
        return false;

    if(!eat(Token(Kind::tok_then, "then")))
        return false;

    std::vector<ASTNode*> instructions = {};
    if(!parseInstruction(instructions))
        return false;

    ASTNode* ifNode = new CompoundNode(instructions);

    ASTNode* elseNode = nullptr;
    if(lexer->peekNextToken().getKind() == Kind::tok_else) {
        if(!eat(Token(Kind::tok_else, "else")))
            return false;

        std::vector<ASTNode*> elseInstructions = {};
        if(!parseInstruction(elseInstructions))
            return false;
        elseNode = new CompoundNode(elseInstructions);
    }

    result = new IfElseNode(conditionNode, ifNode, elseNode);

    return true;
}

bool Parser::parseCondition(ASTNode *&result) {
    ASTNode* leftExpression;
    ASTNode* rightExpression;

    if(!parseExpression(leftExpression))
        return false;

    std::string relationOperator;
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_init:
            eat(Token(Kind::tok_init, "="));
            relationOperator = "=";
            break;
        default:
            return logError("Expected relation operator");
    }

    if(!parseExpression(rightExpression))
        return false;

    result = new BinOpNode(leftExpression, rightExpression, relationOperator);

    return true;
}

bool Parser::parseAssign(ASTNode *&result, Token identifier) {
//    std::cout << "Parsing assign" << std::endl;
    if(!eat(Token(Kind::tok_assign, ":=")))
        return false;

    ASTNode* valueNode = nullptr;
    if(!parseExpression(valueNode))
        return false;

    result = new AssignNode(identifier.getValue(), valueNode);

    return true;
}

bool Parser::parseProcedure(ASTNode *&result, Token identifier) {
//    std::cout << "Parsing procedure" << std::endl;
    if(!eat(Token(Kind::tok_left_paren, "(")))
        return false;

    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_right_paren:
            result = new ProcedureNode(lexer->getToken().getValue());
            return eat(Token(Kind::tok_right_paren, ")")) && eat(Token(Kind::tok_divider, ";"));
        default:
            // TODO: Multiple parameters
            ASTNode* parameter = nullptr;
            if(!parseExpression(parameter))
                return false;
            result = new ProcedureNode(identifier.getValue(), {parameter});
            return eat(Token(Kind::tok_right_paren, ")")) && eat(Token(Kind::tok_divider, ";"));
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

    Token number = lexer->getToken();
//    std::cout << number.getValue() << std::endl;
    if(number.getKind() != Kind::tok_number)
        return logError("Expected number");

    auto numberNode = new IntNode(std::atoi(number.getValue().c_str()));

    result = new ConstNode(identifier.getValue(), numberNode);

    if(lexer->getToken().getKind() != Kind::tok_divider)
        return logError("Missing ;");

    return true;
}

bool Parser::parseVarDeclaration(ASTNode *&result) {
    if(!eat(Token(Kind::tok_var, "var")))
        return false;
    std::vector<ASTNode*> varDeclarations = {};
    if(!parseRestVarDeclaration(varDeclarations))
        return false;

    result = new CompoundNode(varDeclarations);
    return true;
}

bool Parser::parseRestVarDeclaration(std::vector<ASTNode *>&result) {
    if(lexer->peekNextToken().getKind() != Kind::tok_identifier)
        return true;

    Token identifier;
    if(!readIdentifier(identifier))
        return false;
    if(!eat(Token(Kind::tok_type, ":")))
        return false;

    Token type;
    if(!readIdentifier(type))
        return false;

    result.push_back(new VarNode(identifier.getValue(), type.getValue()));

    if(!eat(Token(Kind::tok_divider, ";")))
        return false;

    parseRestVarDeclaration(result);

    return true;
}

bool Parser::parseExpression(ASTNode*& result) {
//    std::cout << "Parsing expression" << std::endl;
    ASTNode* termNode;
    if(!parseTerm(termNode))
        return false;
    return parseRestExpression(termNode, result);
}

bool Parser::parseRestExpression(ASTNode* previousExpression, ASTNode *&result) {
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_plus:
            break;
        case Kind::tok_minus:
            break;
        case Kind::tok_mod:
            break;
        default:
            result = previousExpression;
            return true;
    }

    std::string operatorToken = lexer->getToken().getValue();
    ASTNode* term;
    if(!parseTerm(term))
        return false;
    ASTNode* binOp = new BinOpNode(previousExpression, term, operatorToken);
    return parseRestExpression(binOp, result);
}

bool Parser::parseTerm(ASTNode*& result) {
    ASTNode* factorNode;
    if(!parseFactor(factorNode))
        return false;
    return parseRestTerm(factorNode, result);
}

bool Parser::parseRestTerm(ASTNode *previousTerm, ASTNode *&result) {
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_multiplier:
            break;
        case Kind::tok_division:
            break;
        default:
            result = previousTerm;
            return true;
    }
    std::string operatorToken = lexer->getToken().getValue();
    ASTNode* factor;
    parseFactor(factor);
    ASTNode* binOp = new BinOpNode(previousTerm, factor, operatorToken);
    return parseRestTerm(binOp, result);
}

bool Parser::parseFactor(ASTNode*& result) {
//    std::cout << "Parsing factor" << std::endl;
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_number: {
            int value = std::atoi(lexer->getToken().getValue().c_str());
            result = new IntNode(value);
            return true;
        }
        case Kind::tok_identifier:
            result = new IdentifierNode(lexer->getToken().getValue());
            return true;
        case Kind::tok_left_paren:
            eat(Token(Kind::tok_left_paren, "("));
            if(!parseExpression(result))
                return false;
            return eat(Token(Kind::tok_right_paren, ")"));
        default: {
            std::stringstream ss;
            ss << "Unexpected token " << lexer->getToken().getValue();
            return logError(ss.str());
        }
    }
}

bool Parser::Parse() {
//    std::cout << "Parsing program" << std::endl;
    return parseProgram();
}

Token Parser::getNextToken() {
    return lexer->getToken();
}
