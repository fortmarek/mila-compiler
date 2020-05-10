//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_ASSIGNNODE_H
#define SIMPLEFRONTEND_ASSIGNNODE_H

#include "ASTNode.h"

class AssignNode: public ASTNode {
public:
    AssignNode(std::string identifier, ASTNode* valueNode);
    const void print() override;
private:
    std::string identifier;
    ASTNode* valueNode;
};


#endif //SIMPLEFRONTEND_ASSIGNNODE_H
