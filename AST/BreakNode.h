//
// Created by Marek Fořt on 2020-06-14.
//

#ifndef SIMPLEFRONTEND_BREAKNODE_H
#define SIMPLEFRONTEND_BREAKNODE_H

#include "ASTNode.h"

class BreakNode: public ASTNode {
public:
    BreakNode() = default;
    const void print() override;
};


#endif //SIMPLEFRONTEND_BREAKNODE_H
