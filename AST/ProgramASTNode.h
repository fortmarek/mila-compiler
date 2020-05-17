//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_PROGRAMASTNODE_H
#define SIMPLEFRONTEND_PROGRAMASTNODE_H

#include <string>
#include "ASTNode.h"

class ProgramASTNode: public ASTNode {
public:
    explicit ProgramASTNode(std::string name, ASTNode* declarations, ASTNode* main);
    ProgramASTNode(const ProgramASTNode &programNode);
    ASTNode* getDeclarations();
    ASTNode* getMain();
    const void print() override;
private:
    std::string name;
    ASTNode* declarations;
    ASTNode* main;
};


#endif //SIMPLEFRONTEND_PROGRAMASTNODE_H
