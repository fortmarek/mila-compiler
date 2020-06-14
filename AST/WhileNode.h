//
// Created by Marek Fořt on 2020-06-14.
//

#ifndef SIMPLEFRONTEND_WHILENODE_H
#define SIMPLEFRONTEND_WHILENODE_H

#include "ASTNode.h"

class WhileNode: public ASTNode {
public:
    WhileNode(ASTNode* condition, ASTNode* body);
    ASTNode* getCondition();
    ASTNode* getBody();
    const void print() override;
private:
    ASTNode* condition;
    ASTNode* body;
};


#endif //SIMPLEFRONTEND_WHILENODE_H
