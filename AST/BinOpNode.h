//
// Created by Marek Fořt on 2020-05-17.
//

#ifndef SIMPLEFRONTEND_BINOPNODE_H
#define SIMPLEFRONTEND_BINOPNODE_H

#include "ASTNode.h"

class BinOpNode: public ASTNode {
public:
    BinOpNode(ASTNode* leftOperator, ASTNode* rightOperator, std::string operationToken);
    std::string getOperationToken();
    ASTNode* getLeftOperator();
    ASTNode* getRightOperator();
    const void print() override;
private:
    std::string operationToken;
    ASTNode* leftOperator;
    ASTNode* rightOperator;
};


#endif //SIMPLEFRONTEND_BINOPNODE_H
