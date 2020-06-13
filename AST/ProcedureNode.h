//
// Created by Marek Fořt on 2020-05-10.
//

#ifndef SIMPLEFRONTEND_PROCEDURENODE_H
#define SIMPLEFRONTEND_PROCEDURENODE_H

#include "ASTNode.h"

class ProcedureNode: public ASTNode {
public:
    ProcedureNode(std::string identifier, std::vector<ASTNode*> parameters);
    explicit ProcedureNode(std::string identifier);
    std::vector<ASTNode*> getParameters();
    std::string getIdentifier();
    const void print() override;
private:
    std::string identifier;
    std::vector<ASTNode*> parameters;
};


#endif //SIMPLEFRONTEND_PROCEDURENODE_H
