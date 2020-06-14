//
// Created by Marek Fořt on 2020-06-14.
//

#ifndef SIMPLEFRONTEND_FORNODE_H
#define SIMPLEFRONTEND_FORNODE_H

#include "ASTNode.h"

class ForNode: public ASTNode {
public:
    ForNode(std::string identifier, ASTNode* start, ASTNode* end, ASTNode* body);
    const void print() override;
    ASTNode* getStart();
    ASTNode* getEnd();
    ASTNode* getBody();
    std::string getIdentifier();
private:
    ASTNode* start;
    ASTNode* end;
    ASTNode* body;
    std::string identifier;
};


#endif //SIMPLEFRONTEND_FORNODE_H
