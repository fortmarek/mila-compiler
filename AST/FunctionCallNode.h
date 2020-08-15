//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_FUNCTIONCALLNODE_H
#define SIMPLEFRONTEND_FUNCTIONCALLNODE_H

#include "ASTNode.h"

class FunctionCallNode: public ASTNode {
public:
    FunctionCallNode(std::string identifier, std::vector<ASTNode*> parameters);
    explicit FunctionCallNode(std::string identifier);
    std::vector<ASTNode*> getParameters();
    std::string getIdentifier();
    const void print() override;
private:
    std::string identifier;
    std::vector<ASTNode*> parameters;
};


#endif //SIMPLEFRONTEND_FUNCTIONCALLNODE_H
