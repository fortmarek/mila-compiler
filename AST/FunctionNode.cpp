//
// Created by Marek Fořt on 2020-08-10.
//

#include "FunctionNode.h"

FunctionNode::FunctionNode(
        std::string identifier,
        std::vector<std::pair<std::string, MilaType>> parameters,
        MilaType returnType,
        CompoundNode* block
) :identifier(std::move(identifier)),
   parameters(std::move(parameters)),
   returnType(returnType),
   block(block) {}


const void FunctionNode::print() {
    std::cout << "Function node: " << identifier << std::endl;
    for(auto const& parameter: parameters)
        std::cout << "Parameter: " << parameter.first << std::endl;
    block->print();
}

const MilaType FunctionNode::getReturnType() {
    return returnType;
}

const std::vector<std::pair<std::string, MilaType>> FunctionNode::getParameters() {
    return parameters;
}

const std::string FunctionNode::getIdentifier() {
    return identifier;
}

CompoundNode* FunctionNode::getBlock() {
    return block;
}