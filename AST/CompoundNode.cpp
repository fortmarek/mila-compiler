//
// Created by Marek Fořt on 2020-05-10.
//

#include "CompoundNode.h"

CompoundNode::CompoundNode(std::vector<ASTNode*> children) :children(std::move(children)) {}

const void CompoundNode::print() {
    std::cout << "Compound node:" << std::endl;
    for(auto const& child: children)
        child->print();
}