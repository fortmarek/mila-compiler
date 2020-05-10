//
// Created by Marek Fořt on 2020-05-10.
//

#include "IdentifierNode.h"

IdentifierNode::IdentifierNode(std::string identifier) :identifier(std::move(identifier)) {}

const void IdentifierNode::print() {
    std::cout << "Identifier node: " << std::endl;
    std::cout << "Identifier: " << std::endl;
}