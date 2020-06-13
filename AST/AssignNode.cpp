//
// Created by Marek Fořt on 2020-05-10.
//

#include "AssignNode.h"

AssignNode::AssignNode(std::string identifier, ASTNode *valueNode) :identifier(std::move(identifier)), valueNode(valueNode) {}

const void AssignNode::print() {
    std::cout << "Assign node: " << std::endl;
    std::cout << "\tidentifier: " << identifier << std::endl;
    valueNode->print();
}

std::string AssignNode::getIdentifier() {
    return identifier;
}

ASTNode* AssignNode::getValue() {
    return valueNode;
}