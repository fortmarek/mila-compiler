//
// Created by Marek Fořt on 2020-05-10.
//

#include <vector>
#include "ProcedureNode.h"

ProcedureNode::ProcedureNode(std::string identifier, std::vector<ASTNode *> parameters):
identifier(std::move(identifier)),
parameters(std::move(parameters)) {}

ProcedureNode::ProcedureNode(std::string identifier) :identifier(std::move(identifier)) {}

std::vector<ASTNode*> ProcedureNode::getParameters() {
    return parameters;
}

const void ProcedureNode::print() {
    std::cout << "ProcedureNode: " << std::endl;
    std::cout << identifier << std::endl;
    for(auto const& parameter: parameters)
        parameter->print();
}