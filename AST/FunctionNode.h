//
// Created by Marek Fořt on 2020-08-10.
//

#ifndef SIMPLEFRONTEND_FUNCTIONNODE_H
#define SIMPLEFRONTEND_FUNCTIONNODE_H

#include "ASTNode.h"
#include "CompoundNode.h"
#include <utility>

class FunctionNode: public ASTNode {
public:
    FunctionNode(
            std::string identifier,
            std::vector<std::pair<std::string, MilaType>> parameters,
            MilaType returnType,
            CompoundNode* block
    );
    const void print() override;
    const MilaType getReturnType();
    const std::vector<std::pair<std::string, MilaType>> getParameters();
    const std::string getIdentifier();
    CompoundNode* getBlock();
private:
    std::string identifier;
    std::vector<std::pair<std::string, MilaType>> parameters;
    MilaType returnType;
    CompoundNode* block;
};


#endif //SIMPLEFRONTEND_FUNCTIONNODE_H
