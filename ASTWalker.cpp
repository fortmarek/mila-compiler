//
// Created by Marek Fořt on 2020-05-17.
//

#include "ASTWalker.h"
#include "AST/ASTNode.h"
#include "AST/ProgramASTNode.h"
#include "AST/CompoundNode.h"
#include "AST/ConstNode.h"
#include "AST/VarNode.h"
#include "AST/IntNode.h"
#include "AST/MainNode.h"
#include "Parser.hpp"

ASTWalker::ASTWalker() :milaContext(), milaBuilder(milaContext), milaModule("mila", milaContext) {}

const Module& ASTWalker::generate(ASTNode* program) {

    // create writeln function
    {
        std::vector<Type*> Ints(1, Type::getInt32Ty(milaContext));
        FunctionType * FT = FunctionType::get(Type::getInt32Ty(milaContext), Ints, false);
        Function * F = Function::Create(FT, Function::ExternalLinkage, "writeln", milaModule);
        for (auto & Arg : F->args())
            Arg.setName("x");
    }

    // create readln function
    {
        std::vector<Type*> Ints(1, Type::getInt32Ty(milaContext));
        FunctionType * FT = FunctionType::get(milaBuilder.getVoidTy(), Ints, false);
        Function * F = Function::Create(FT, Function::ExternalLinkage, "readln", milaModule);
        for (auto & Arg : F->args())
            Arg.setName("x");
    }

    program->walk(this);

    return this->milaModule;
}

llvm::Value* ASTWalker::visit(ProgramASTNode* program) {
    program->getDeclarations()->walk(this);
    program->getMain()->walk(this);
    return nullptr;
}

llvm::Value* ASTWalker::visit(MainNode *mainNode) {
    FunctionType * FT = FunctionType::get(Type::getInt32Ty(milaContext), false);
    Function * MainFunction = Function::Create(FT, Function::ExternalLinkage, "main", milaModule);

    // block
    BasicBlock * BB = BasicBlock::Create(milaContext, "entry", MainFunction);
    milaBuilder.SetInsertPoint(BB);

    milaBuilder.CreateCall(milaModule.getFunction("writeln"), {
        ConstantInt::get(milaContext, APInt(32, 42))
    });

    visit(dynamic_cast<CompoundNode*>(mainNode));

    // call writeln with value from lexel
//        milaBuilder.CreateCall(milaModule.getFunction("writeln"), {
//                ConstantInt::get(milaContext, APInt(32, lexer->numVal()))
//        });

    // return 0
    milaBuilder.CreateRet(ConstantInt::get(Type::getInt32Ty(milaContext), 0));
    return nullptr;
}

llvm::Value* ASTWalker::visit(CompoundNode *compound) {
    for(auto const& child: compound->getChildren())
        child->walk(this);
    return nullptr;
}

llvm::Value* ASTWalker::visit(ConstNode *constNode) {
    llvm::Value* value = constNode->getValue()->walk(this);
    symbolTable[constNode->getIdentifier()] = value;
    milaBuilder.CreateFAdd(value, value, "addtmp");
    return nullptr;
}

llvm::Value* ASTWalker::visit(IntNode *intNode) {
    return llvm::ConstantInt::get(milaContext, llvm::APInt(64, intNode->getValue()));
}

llvm::Value* ASTWalker::visit(VarNode *varNode) {
    return nullptr;
}