//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_PROGRAMASTNODE_H
#define SIMPLEFRONTEND_PROGRAMASTNODE_H

#include <string>
#include "ASTNode.h"

class ProgramASTNode: public ASTNode {
public:
    ProgramASTNode(std::string name);
    void print();
private:
    std::string name;
};


#endif //SIMPLEFRONTEND_PROGRAMASTNODE_H
