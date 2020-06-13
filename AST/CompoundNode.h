//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_COMPOUNDNODE_H
#define SIMPLEFRONTEND_COMPOUNDNODE_H

#include "ASTNode.h"
#include <vector>

class CompoundNode: public ASTNode {
public:
    explicit CompoundNode(std::vector<ASTNode*> children);
    std::vector<ASTNode*> getChildren();
    const void print() override;
private:
    std::vector<ASTNode*> children;
};


#endif //SIMPLEFRONTEND_COMPOUNDNODE_H
