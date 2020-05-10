//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_VARNODE_H
#define SIMPLEFRONTEND_VARNODE_H

#include "ASTNode.h"

class VarNode: public ASTNode {
public:
    VarNode(std::string identifier, std::string type);
    const void print() override;
private:
    std::string identifier;
    std::string type;
};




#endif //SIMPLEFRONTEND_VARNODE_H
