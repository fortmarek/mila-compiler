//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_INTNODE_H
#define SIMPLEFRONTEND_INTNODE_H

#include "ASTNode.h"

class IntNode: public ASTNode {
public:
    explicit IntNode(int value);
    const void print() override;
    int getValue();
private:
    int value;
};


#endif //SIMPLEFRONTEND_INTNODE_H
