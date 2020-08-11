//
// Created by Marek Fořt on 2020-08-10.
//

#ifndef SIMPLEFRONTEND_FUNCTIONNODE_H
#define SIMPLEFRONTEND_FUNCTIONNODE_H

#include "ASTNode.h"
#include <utility>

class FunctionNode: public ASTNode {
public:
    FunctionNode(
            std::string identifier,
            std::vector<std::pair<std::string, MilaType>> parameters,
            MilaType returnType,
            std::vector<ASTNode*> calls
    );
    const void print() override;
private:
    std::string identifier;
    std::vector<std::pair<std::string, MilaType>> parameters;
    MilaType returnType;
    std::vector<ASTNode*> calls;
};


#endif //SIMPLEFRONTEND_FUNCTIONNODE_H
