#ifndef SIMPLEFRONTEND_ASTNODE_H
#define SIMPLEFRONTEND_ASTNODE_H

#include <iostream>

class ASTNode {
public:
    const virtual void print() = 0;
};

#endif //SIMPLEFRONTEND_ASTNODE_H
