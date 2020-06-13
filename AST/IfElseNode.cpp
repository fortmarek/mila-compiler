//
// Created by Marek Fořt on 2020-06-13.
//

#include "IfElseNode.h"

IfElseNode::IfElseNode(ASTNode *conditionNode, ASTNode *ifNode, ASTNode *elseNode) :conditionNode(conditionNode), ifNode(ifNode), elseNode(elseNode) {}

const void IfElseNode::print() {
    std::cout << "If else node:" << std::endl;
    conditionNode->print();
    ifNode->print();
    if(elseNode) {
        elseNode->print();
    }
}

ASTNode* IfElseNode::getConditionNode() {
    return conditionNode;
}

ASTNode* IfElseNode::getIfNode() {
    return ifNode;
}

ASTNode* IfElseNode::getElseNode() {
    return elseNode;
}