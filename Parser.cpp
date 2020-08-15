#include <sstream>
#include "Parser.hpp"
#include "AST/ProgramASTNode.h"
#include "AST/IntNode.h"
#include "AST/ConstNode.h"
#include "AST/CompoundNode.h"
#include "AST/VarNode.h"
#include "AST/FunctionCallNode.h"
#include "AST/AssignNode.h"
#include "AST/IdentifierNode.h"
#include "AST/BinOpNode.h"
#include "AST/MainNode.h"
#include "AST/ForNode.h"
#include "AST/IfElseNode.h"
#include "AST/WhileNode.h"
#include "AST/BreakNode.h"
#include "AST/FunctionDeclarationNode.h"

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

    std::vector<ASTNode*> methodDeclarations = {};
    if(!parseMethodDeclaration(methodDeclarations))
        return false;

    auto methodDeclarationsNode = new CompoundNode(methodDeclarations);

    std::vector<ASTNode*> instructions = {};
    if(!parseDeclaration(instructions))
        return false;

    if(!parseBlock(instructions))
        return false;
    auto mainNode = new MainNode(instructions);

    auto programNode = new ProgramASTNode(identifierToken.getValue(), methodDeclarationsNode, mainNode);

//    programNode->print();

    if(!eat(Token(Kind::tok_dot, ".")))
        return false;

    program = programNode;

    return true;
}

bool Parser::parseMethodDeclaration(std::vector<ASTNode *> &result) {
    ASTNode* methodNode;
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_function:
            if(!parseFunctionDeclaration(methodNode))
                return false;
            break;
        case Kind::tok_procedure:
            if(!parseProcedureDeclaration(methodNode))
                return false;
            break;
        default:
            return true;
    }

    result.push_back(methodNode);

    return parseMethodDeclaration(result);
}

bool Parser::parseFunctionDeclaration(ASTNode *&result) {
    if(!eat(Token(Kind::tok_function, "function")))
        return false;

    Token identifier;
    if(!readIdentifier(identifier))
        return false;

    if(!eat(Token(Kind::tok_left_paren, "(")))
        return false;

    std::vector<std::pair<std::string, MilaType>> parameters;
    if(!parseParameterDeclaration(parameters))
        return false;

    if(!eat(Token(Kind::tok_right_paren, ")")))
        return false;

    if(!eat(Token(Kind::tok_type, ":")))
        return false;

    MilaType returnType;
    if(!readType(returnType))
        return false;

    if(!eat(Token(Kind::tok_divider, ";")))
        return false;

    std::vector<ASTNode*> instructions = {};
    if(!parseBlock(instructions))
        return false;

    if(!eat(Token(Kind::tok_divider, ";")))
        return false;

    result = new FunctionDeclarationNode(
            identifier.getValue(),
            parameters,
            returnType,
            new CompoundNode(instructions)
    );

    return true;
}

bool Parser::parseProcedureDeclaration(ASTNode *&result) {
    if(!eat(Token(Kind::tok_procedure, "procedure")))
        return false;

    Token identifier;
    if(!readIdentifier(identifier))
        return false;

    if(!eat(Token(Kind::tok_left_paren, "(")))
        return false;

    std::vector<std::pair<std::string, MilaType>> parameters;
    if(!parseParameterDeclaration(parameters))
        return false;

    if(!eat(Token(Kind::tok_right_paren, ")")))
        return false;

    if(!eat(Token(Kind::tok_divider, ";")))
        return false;

    std::vector<ASTNode*> instructions;

    if(!parseDeclaration(instructions))
        return false;

    if(!parseBlock(instructions))
        return false;

    if(!eat(Token(Kind::tok_divider, ";")))
        return false;

    result = new FunctionDeclarationNode(
            identifier.getValue(),
            parameters,
            MilaType::void_type,
            new CompoundNode(instructions)
    );

    return true;
}

bool Parser::parseParameterDeclaration(std::vector<std::pair<std::string, MilaType>> &parameters) {
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_identifier:
            break;
        default:
            return true;
    }

    Token parameterIdentifier;
    if(!readIdentifier(parameterIdentifier))
        return false;

    if(!eat(Token(Kind::tok_type, ":")))
        return false;

    MilaType type;
    if(!readType(type))
        return false;

    parameters.emplace_back(parameterIdentifier.getValue(), type);

    return parseRestParameterDeclaration(parameters);
}

bool Parser::parseRestParameterDeclaration(std::vector<std::pair<std::string, MilaType>> &parameters) {
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_comma:
            if(!eat(Token(Kind::tok_comma, ",")))
                return false;
            break;
        default:
            return true;
    }

    Token parameterIdentifier;
    if(!readIdentifier(parameterIdentifier))
        return false;

    if(!eat(Token(Kind::tok_type, ":")))
        return false;

    MilaType type;
    if(!readType(type))
        return false;

    parameters.emplace_back(parameterIdentifier.getValue(), type);

    return parseRestParameterDeclaration(parameters);
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
    if(!parseInstructions(result))
        return false;
    return eat(Token(Kind::tok_end, "end"));
}

bool Parser::parseInstructions(std::vector<ASTNode *> &result) {
    if(!parseInstruction(result))
        return false;

    return parseRestInstruction(result);
}

bool Parser::parseInstruction(std::vector<ASTNode *> &result) {
    ASTNode* currentNodeResult = nullptr;
    std::vector<ASTNode*> instructions = {};
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_if:
            if(!parseIfBlock(currentNodeResult))
                return false;
            break;
        case Kind::tok_for:
            if(!parseForBlock(currentNodeResult))
                return false;
            break;
        case Kind::tok_while:
            if(!parseWhileBlock(currentNodeResult))
                return false;
            break;
        case Kind::tok_break:
            eat(Token(Kind::tok_break, "break"));
            currentNodeResult = new BreakNode();
            break;
        case Kind::tok_begin:
            if(!parseBlock(instructions))
                return false;
            currentNodeResult = new CompoundNode(instructions);
            break;
        default:
            Token identifier;
            if(!readIdentifier(identifier))
                return false;
            switch(lexer->peekNextToken().getKind()) {
                case Kind::tok_left_paren:
                    if(!parseFunctionCall(currentNodeResult, identifier))
                        return false;
                    break;
                case Kind::tok_assign:
                    if(!parseAssign(currentNodeResult, identifier))
                        return false;
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

    return true;
}

bool Parser::parseRestInstruction(std::vector<ASTNode *> &result) {
    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_divider:
            break;
        default:
            return true;
    }

    if(!eat(Token(Kind::tok_divider, ";")))
        return false;

    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_end:
            return true;
        default:
            break;
    }

    if(!parseInstruction(result))
        return false;

    return parseRestInstruction(result);
}

bool Parser::parseWhileBlock(ASTNode *&result) {
    if(!eat(Token(Kind::tok_while, "while")))
        return false;

    ASTNode* conditionNode;
    if(!parseCondition(conditionNode))
        return false;

    if(!eat(Token(Kind::tok_do, "do")))
        return false;

    if(!eat(Token(Kind::tok_begin, "begin")))
        return false;

    std::vector<ASTNode*> instructions = {};
    if(!parseInstructions(instructions))
        return false;

    ASTNode* body = new CompoundNode(instructions);

    result = new WhileNode(conditionNode, body);

    return eat(Token(Kind::tok_end, "end"));
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

    ForType type;

    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_to:
            eat(Token(Kind::tok_to, "to"));
            type = ForType::to;
            break;
        case Kind::tok_downto:
            eat(Token(Kind::tok_downto, "downto"));
            type = ForType::downto;
            break;
        default:
            return logError("Expected to or downto");
    }

    ASTNode* endNode;
    if(!parseExpression(endNode))
        return false;

    if(!eat(Token(Kind::tok_do, "do")))
        return false;

    if(!eat(Token(Kind::tok_begin, "begin")))
        return false;

    std::vector<ASTNode*> bodyInstructions = {};
    if(!parseInstructions(bodyInstructions))
        return false;

    if(!eat(Token(Kind::tok_end, "end")))
        return false;

//    if(!eat(Token(Kind::tok_divider, ";")))
//        return false;

    ASTNode* bodyNode = new CompoundNode(bodyInstructions);

    result = new ForNode(identifier.getValue(), type, startNode, endNode, bodyNode);

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
    bool isParenthesised = false;

    if(lexer->peekNextToken().getKind() == Kind::tok_left_paren) {
        isParenthesised = true;
        if(!eat(Token(Kind::tok_left_paren, "(")))
            return false;
    }

    ASTNode* leftExpression;
    ASTNode* rightExpression;

    if(!parseExpression(leftExpression))
        return false;

    std::string relationOperator;
    switch(lexer->getToken().getKind()) {
        case Kind::tok_init:
            relationOperator = "=";
            break;
        case Kind::tok_notequal:
            relationOperator = "<>";
            break;
        case Kind::tok_greaterequal:
            relationOperator = ">=";
            break;
        case Kind::tok_lessequal:
            relationOperator = "=<";
            break;
        case Kind::tok_less:
            relationOperator = "<";
            break;
        case Kind::tok_greater:
            relationOperator = ">";
            break;
        default:
            return logError("Expected relation operator");
    }

    if(!parseExpression(rightExpression))
        return false;

    if(isParenthesised) {
        if(!eat(Token(Kind::tok_right_paren, ")")))
            return false;
    }

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

bool Parser::parseFunctionCall(ASTNode *&result, Token identifier) {
    //    std::cout << "Parsing procedure" << std::endl;
    if(!eat(Token(Kind::tok_left_paren, "(")))
        return false;

    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_right_paren:
            result = new FunctionCallNode(lexer->getToken().getValue());
            return eat(Token(Kind::tok_right_paren, ")")) && eat(Token(Kind::tok_divider, ";"));
        default:
            // TODO: Multiple parameters
            ASTNode* parameter = nullptr;
            if(!parseExpression(parameter))
                return false;
            result = new FunctionCallNode(identifier.getValue(), {parameter});
            return eat(Token(Kind::tok_right_paren, ")"));
    }
}

bool Parser::eat(Token token) {
    Token currentToken = lexer->getToken();
    if(currentToken.getKind() != token.getKind()) {
        std::stringstream ss;
        ss << "Unexpected token " << currentToken.getValue() << ", expected: " << token.getValue();
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

//bool Parser::parseParameterIdentifiers(std::vector<std::string> &parameters) {
//    Token identifier;
//    if(!readIdentifier(identifier))
//        return false;
//
//    parameters.push_back(identifier.getValue());
//
//    return parseRestParameterIdentifiers(parameters);
//}
//
//bool Parser::parseRestParameterIdentifiers(std::vector<std::string> &parameters) {
//    switch(lexer->peekNextToken().getKind()) {
//        case Kind::tok_comma:
//            eat(Token(Kind::tok_comma, ","));
//            break;
//        default:
//            return true;
//    }
//
//    Token identifier;
//    if(!readIdentifier(identifier))
//        return false;
//
//    parameters.push_back(identifier.getValue());
//
//    return parseRestParameterIdentifiers(parameters);
//}


bool Parser::parseVarDeclaration(ASTNode *&result) {
    if(!eat(Token(Kind::tok_var, "var")))
        return false;

    std::vector<std::string> varIdentifiers;

    Token identifier;
    if(!readIdentifier(identifier))
        return false;

    varIdentifiers.push_back(identifier.getValue());

    if(!parseRestVarDeclaration(varIdentifiers))
        return false;

    if(!eat(Token(Kind::tok_type, ":")))
        return false;

    MilaType type;
    if(!readType(type))
        return false;

    Token tokenType;
    switch(type) {
        case MilaType::integer:
            tokenType = Token(Kind::tok_integer, "integer");
            break;
        case MilaType::void_type:
            return logError("Unexpected void keyword");
    }

    if(!eat(Token(Kind::tok_divider, ";")))
        return false;

    std::vector<ASTNode*> varDeclarations;
    for(auto const& identifier: varIdentifiers) {
        varDeclarations.push_back(new VarNode(identifier, tokenType.getValue()));
    }

    result = new CompoundNode(varDeclarations);
    return true;
}

bool Parser::parseRestVarDeclaration(std::vector<std::string>&result) {
    if(lexer->peekNextToken().getKind() != Kind::tok_comma)
        return true;

    eat(Token(Kind::tok_comma, ","));

    Token identifier;
    if(!readIdentifier(identifier))
        return false;

    result.push_back(identifier.getValue());

    return parseRestVarDeclaration(result);
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
            return parseIdentifier(result);
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

bool Parser::parseIdentifier(ASTNode *&result) {
    Token identifier;
    if(!readIdentifier(identifier))
        return false;

    switch(lexer->peekNextToken().getKind()) {
        case Kind::tok_left_paren:
            return parseFunctionCall(result, identifier);
        default:
            result = new IdentifierNode(identifier.getValue());
            return true;
    }
}

bool Parser::Parse() {
    //    std::cout << "Parsing program" << std::endl;
    return parseProgram();
}

Token Parser::getNextToken() {
    return lexer->getToken();
}

bool Parser::readType(MilaType &type) {
    switch(lexer->getToken().getKind()) {
        case Kind::tok_integer:
            type = MilaType::integer;
            return true;
        default:
            return logError("Expected mila type");
    }
}