//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_IDENTIFIERNODE_H
#define SIMPLEFRONTEND_IDENTIFIERNODE_H

#include "ASTNode.h"

class IdentifierNode: public ASTNode {
public:
    explicit IdentifierNode(std::string identifier);
    std::string getIdentifier();
    const void print() override;
private:
    std::string identifier;
};


#endif //SIMPLEFRONTEND_IDENTIFIERNODE_H
