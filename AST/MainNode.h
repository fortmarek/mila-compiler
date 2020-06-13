//
// Created by Marek Fořt on 2020-06-01.
//

#ifndef SIMPLEFRONTEND_MAINNODE_H
#define SIMPLEFRONTEND_MAINNODE_H

#include <vector>
#include "CompoundNode.h"

class MainNode: public CompoundNode {
public:
    explicit MainNode(std::vector<ASTNode*> children);
};


#endif //SIMPLEFRONTEND_MAINNODE_H
