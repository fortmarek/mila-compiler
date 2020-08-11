//
// Created by Marek Fořt on 2020-08-10.
//

#include "FunctionNode.h"

FunctionNode::FunctionNode(
        std::string identifier,
        std::vector<std::pair<std::string, MilaType>> parameters,
        MilaType returnType,
        std::vector<ASTNode *> calls
) :identifier(std::move(identifier)),
   parameters(std::move(parameters)),
   returnType(returnType),
   calls(std::move(calls)) {}


const void FunctionNode::print() {
    std::cout << "Function node: " << identifier << std::endl;
    for(auto const& parameter: parameters)
        std::cout << "Parameter: " << parameter.first << std::endl;
    for(auto const& call: calls)
        call->print();
}