//
// Created by Marek Fořt on 2020-08-10.
//

#include "FunctionDeclarationNode.h"

FunctionDeclarationNode::FunctionDeclarationNode(
        std::string identifier,
        std::vector<std::pair<std::string, MilaType>> parameters,
        MilaType returnType,
        CompoundNode* block
) :identifier(std::move(identifier)),
   parameters(std::move(parameters)),
   returnType(returnType),
   block(block) {}


const void FunctionDeclarationNode::print() {
    std::cout << "Function node: " << identifier << std::endl;
    for(auto const& parameter: parameters)
        std::cout << "Parameter: " << parameter.first << std::endl;
    block->print();
}

const MilaType FunctionDeclarationNode::getReturnType() {
    return returnType;
}

const std::vector<std::pair<std::string, MilaType>> FunctionDeclarationNode::getParameters() {
    return parameters;
}

const std::string FunctionDeclarationNode::getIdentifier() {
    return identifier;
}

CompoundNode* FunctionDeclarationNode::getBlock() {
    return block;
}