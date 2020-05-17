//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_CONSTNODE_H
#define SIMPLEFRONTEND_CONSTNODE_H

#include "ASTNode.h"

class ConstNode: public ASTNode {
public:
    explicit ConstNode(std::string identifier, ASTNode* value);
    const void print() override;
    ASTNode* getValue();
    std::string getIdentifier();
private:
    std::string identifier;
    ASTNode* value;
};


#endif //SIMPLEFRONTEND_CONSTNODE_H
