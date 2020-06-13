//
// Created by Marek Fořt on 2020-05-17.
//

#ifndef SIMPLEFRONTEND_ASTWALKER_H
#define SIMPLEFRONTEND_ASTWALKER_H
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

class ASTNode;
class ProgramASTNode;
class CompoundNode;
class ConstNode;
class VarNode;
class IntNode;
class MainNode;
class ProcedureNode;
class IdentifierNode;

class ASTWalker {
public:
    ASTWalker();
    ~ASTWalker() = default;
    const llvm::Module& generate(ASTNode* program);
    llvm::Value* visit(ProgramASTNode* program);
    llvm::Value* visit(CompoundNode* compound);
    llvm::Value* visit(ConstNode* constNode);
    llvm::Value* visit(VarNode* varNode);
    llvm::Value* visit(IntNode* intNode);
    llvm::Value* visit(MainNode* mainNode);
    llvm::Value* visit(ProcedureNode* procedureNode);
    llvm::Value* visit(IdentifierNode* identifierNode);
private:
    llvm::LLVMContext milaContext;   // llvm context
    llvm::IRBuilder<> milaBuilder;   // llvm builder
    llvm::Module milaModule;         // llvm module
    std::map<std::string, llvm::Value*> symbolTable;
};


#endif //SIMPLEFRONTEND_ASTWALKER_H
