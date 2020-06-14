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
#include "ProcedureNode.h"
#include "IdentifierNode.h"
#include "AssignNode.h"
#include "BinOpNode.h"
#include "IfElseNode.h"
#include "ForNode.h"
#include "WhileNode.h"
#include "BreakNode.h"

const void ASTNode::print() {

}

llvm::Value* ASTNode::walk(ASTWalker *walker) {
    if(auto program = dynamic_cast<ProgramASTNode*>(this))
        return walker->visit(program);
    else if (auto mainNode = dynamic_cast<MainNode*>(this))
        return walker->visit(mainNode);
    else if(auto procedureNode = dynamic_cast<ProcedureNode*>(this))
        return walker->visit(procedureNode);
    else if(auto compound = dynamic_cast<CompoundNode*>(this))
        return walker->visit(compound);
    else if(auto constNode = dynamic_cast<ConstNode*>(this))
        return walker->visit(constNode);
    else if(auto varNode = dynamic_cast<VarNode*>(this))
        return walker->visit(varNode);
    else if (auto intNode = dynamic_cast<IntNode*>(this))
        return walker->visit(intNode);
    else if (auto identifierNode = dynamic_cast<IdentifierNode*>(this))
        return walker->visit(identifierNode);
    else if(auto assignmentNode = dynamic_cast<AssignNode*>(this))
        return walker->visit(assignmentNode);
    else if(auto binOpNode = dynamic_cast<BinOpNode*>(this))
        return walker->visit(binOpNode);
    else if(auto ifElseNode = dynamic_cast<IfElseNode*>(this))
        return walker->visit(ifElseNode);
    else if(auto forNode = dynamic_cast<ForNode*>(this))
        return walker->visit(forNode);
    else if(auto whileNode = dynamic_cast<WhileNode*>(this))
        return walker->visit(whileNode);
    else if(auto breakNode = dynamic_cast<BreakNode*>(this))
        return walker->visit(breakNode);
    return nullptr;
}