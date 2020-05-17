//
// Created by Marek Fořt on 2020-05-10.
//

#include "ConstNode.h"

ConstNode::ConstNode(std::string identifier, ASTNode *value) :identifier(std::move(identifier)), value(value) {}

const void ConstNode::print() {
    std::cout << "Const node: " << std::endl;
    value->print();
}

std::string ConstNode::getIdentifier() {
    return identifier;
}

ASTNode* ConstNode::getValue() {
    return value;
}