//
// Created by Marek Fořt on 2020-05-10.
//

#include "ASTNode.h"
#include "ProgramASTNode.h"
#include "llvm/Support/Casting.h"
#include "CompoundNode.h"
#include "ConstNode.h"
#include "VarNode.h"
#include "IntNode.h"
#include "MainNode.h"

const void ASTNode::print() {

}

llvm::Value* ASTNode::walk(ASTWalker *walker) {
    if(auto program = dynamic_cast<ProgramASTNode*>(this))
        return walker->visit(program);
    else if (auto mainNode = dynamic_cast<MainNode*>(this))
        return walker->visit(mainNode);
    else if(auto compound = dynamic_cast<CompoundNode*>(this))
        return walker->visit(compound);
    else if(auto constNode = dynamic_cast<ConstNode*>(this))
        return walker->visit(constNode);
    else if(auto varNode = dynamic_cast<VarNode*>(this))
        return walker->visit(varNode);
    else if (auto intNode = dynamic_cast<IntNode*>(this))
        return walker->visit(intNode);
    return nullptr;
}