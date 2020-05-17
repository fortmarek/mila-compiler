#ifndef SIMPLEFRONTEND_ASTNODE_H
#define SIMPLEFRONTEND_ASTNODE_H

#include <iostream>
#include "llvm/IR/Value.h"
#include "../ASTWalker.h"

class ASTNode {
public:
    const virtual void print();
    // TODO: Make virtual
    virtual llvm::Value *walk(ASTWalker* walker);
};

#endif //SIMPLEFRONTEND_ASTNODE_H
