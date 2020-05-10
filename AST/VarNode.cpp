//
// Created by Marek Fořt on 2020-05-10.
//

#include "VarNode.h"

VarNode::VarNode(std::string identifier, std::string type) :identifier(std::move(identifier)), type(std::move(type)) {}

const void VarNode::print() {
    std::cout << "Var node: " << std::endl;
    std::cout << "identifier: " << identifier << std::endl;
    std::cout << "type: " << type << std::endl;
}