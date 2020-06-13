//
// Created by Marek Fořt on 2020-06-13.
//

#ifndef SIMPLEFRONTEND_IFELSENODE_H
#define SIMPLEFRONTEND_IFELSENODE_H


#include "ASTNode.h"

class IfElseNode: public ASTNode {
public:
    IfElseNode(ASTNode* conditionNode, ASTNode* ifNode, ASTNode* elseNode);
    const void print() override;
    ASTNode* getConditionNode();
    ASTNode* getIfNode();
    ASTNode* getElseNode();
private:
    ASTNode* conditionNode;
    ASTNode* ifNode;
    ASTNode* elseNode;
};


#endif //SIMPLEFRONTEND_IFELSENODE_H
