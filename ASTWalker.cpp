//
// Created by Marek Fořt on 2020-05-17.
//

#include "ASTWalker.h"
#include "AST/ASTNode.h"
#include "AST/ProgramASTNode.h"
#include "AST/CompoundNode.h"
#include "AST/ConstNode.h"
#include "AST/VarNode.h"
#include "Parser.hpp"

ASTWalker::ASTWalker() :MilaContext(), MilaBuilder(MilaContext), MilaModule("mila", MilaContext) {}

const Module& ASTWalker::generate(ASTNode* program) {

    // create writeln function
    {
        std::vector<Type*> Ints(1, Type::getInt32Ty(MilaContext));
        FunctionType * FT = FunctionType::get(Type::getInt32Ty(MilaContext), Ints, false);
        Function * F = Function::Create(FT, Function::ExternalLinkage, "writeln", MilaModule);
        for (auto & Arg : F->args())
            Arg.setName("x");
    }

    program->walk(this);

    // create main function
//    {
//        FunctionType * FT = FunctionType::get(Type::getInt32Ty(MilaContext), false);
//        Function * MainFunction = Function::Create(FT, Function::ExternalLinkage, "main", MilaModule);
//
//        // block
//        BasicBlock * BB = BasicBlock::Create(MilaContext, "entry", MainFunction);
//        MilaBuilder.SetInsertPoint(BB);
//
//        // call writeln with value from lexel
//        MilaBuilder.CreateCall(MilaModule.getFunction("writeln"), {
//                ConstantInt::get(MilaContext, APInt(32, lexer->numVal()))
//        });
//
//        // return 0
//        MilaBuilder.CreateRet(ConstantInt::get(Type::getInt32Ty(MilaContext), 0));
//    }

    return this->MilaModule;
}

llvm::Value* ASTWalker::visit(ProgramASTNode* program) {
    program->getDeclarations()->walk(this);
//    program->getMain()->walk(this);
    return nullptr;
}

llvm::Value* ASTWalker::visit(CompoundNode *compound) {
    for(auto const& child: compound->getChildren())
        child->walk(this);
    return nullptr;
}

llvm::Value* ASTWalker::visit(ConstNode *constNode) {
    llvm::Value* value = constNode->getValue()->walk(this);
    MilaBuilder.CreateLoad(value, constNode->getIdentifier());
}

llvm::Value* ASTWalker::visit(VarNode *varNode) {

}