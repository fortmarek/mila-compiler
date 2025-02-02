//
// Created by Marek Fořt on 2020-05-17.
//

#include "BinOpNode.h"

BinOpNode::BinOpNode(
        ASTNode *leftOperator,
        ASTNode *rightOperator,
        std::string operationToken
        ):
        leftOperator(leftOperator),
        rightOperator(rightOperator),
        operationToken(std::move(operationToken))
{}

std::string BinOpNode::getOperationToken() {
    return operationToken;
}

ASTNode* BinOpNode::getLeftOperator() {
    return leftOperator;
}

ASTNode* BinOpNode::getRightOperator() {
    return rightOperator;
}

const void BinOpNode::print() {
    std::cout << "BinOpNode: " << std::endl;
    std::cout << operationToken << std::endl;
    std::cout << "left operator: " << std::endl;
    leftOperator->print();
    std::cout << "right operator: " << std::endl;
    rightOperator->print();
}