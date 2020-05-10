//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_BLOCKNODE_H
#define SIMPLEFRONTEND_BLOCKNODE_H

#include <vector>
#include "ASTNode.h"

class BlockNode: public ASTNode {
public:
    BlockNode(std::vector<ASTNode*> instructions);
    const void print() override;
private:
    std::vector<ASTNode*> instructions;
};


#endif //SIMPLEFRONTEND_BLOCKNODE_H
