//
// Created by Marek Fořt on 2020-08-15.
//

#ifndef SIMPLEFRONTEND_EXITNODE_H
#define SIMPLEFRONTEND_EXITNODE_H

#include "ASTNode.h"

class ExitNode: public ASTNode {
public:
    ExitNode() = default;
    const void print() override;
};


#endif //SIMPLEFRONTEND_EXITNODE_H
