//
// Created by Marek Fořt on 2020-05-10.
//

#include <vector>
#include "FunctionCallNode.h"

FunctionCallNode::FunctionCallNode(std::string identifier, std::vector<ASTNode *> parameters):
identifier(std::move(identifier)),
parameters(std::move(parameters)) {}

FunctionCallNode::FunctionCallNode(std::string identifier) :identifier(std::move(identifier)) {}

std::vector<ASTNode*> FunctionCallNode::getParameters() {
    return parameters;
}

std::string FunctionCallNode::getIdentifier() {
    return identifier;
}

const void FunctionCallNode::print() {
    std::cout << "FunctionCallNode: " << std::endl;
    std::cout << identifier << std::endl;
    for(auto const& parameter: parameters)
        parameter->print();
}