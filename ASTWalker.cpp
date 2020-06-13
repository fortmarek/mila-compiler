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
#include "AST/ProcedureNode.h"
#include "AST/IdentifierNode.h"
#include "AST/AssignNode.h"
#include "AST/BinOpNode.h"
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
        std::vector<Type*> Ints(1, Type::getInt32PtrTy(milaContext));
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

    // create readln function
//    std::vector<Type*> Ints(1, Type::getInt32Ty(milaContext));
//    FunctionType * FTT = FunctionType::get(milaBuilder.getVoidTy(), Ints, false);
//    Function * F = Function::Create(FTT, Function::ExternalLinkage, "readln", milaModule);
//    for (auto & Arg : F->args()) {
//        Arg.setName("y");
//        std::cout << "olaa" << std::endl;
//        // Create an alloca for this variable.
//        AllocaInst *Alloca = ASTWalker::createEntryBlockAlloca(F, Type::getInt32Ty(milaContext), Arg.getName());
//        std::cout << "olaa" << std::endl;
//        // Store the initial value into the alloca.
//        milaBuilder.CreateStore(&Arg, Alloca);
//
//        // Add arguments to variable symbol table.
//        symbolTable[Arg.getName()] = Alloca;
//    }

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
    return value;
}

llvm::Value* ASTWalker::visit(IntNode *intNode) {
    return llvm::ConstantInt::get(milaContext, llvm::APInt(32, intNode->getValue()));
}

llvm::Value* ASTWalker::visit(VarNode *varNode) {
    Function *function = milaBuilder.GetInsertBlock()->getParent();
    std::string identifier = varNode->getIdentifier();
    AllocaInst* alloca = ASTWalker::createEntryBlockAlloca(function, Type::getInt32Ty(milaContext), identifier);
    symbolTable[identifier] = alloca;
    return alloca;
}

llvm::Value* ASTWalker::visit(ProcedureNode *procedureNode) {
    std::vector<Value*> parameters = {};
    Function* function = milaModule.getFunction(procedureNode->getIdentifier());
    auto parametersIterator = procedureNode->getParameters().begin();
    for(auto const& arg: function->args()) {
        llvm::Value* value = (*parametersIterator)->walk(this);
        if(arg.getType()->isPointerTy())
            parameters.push_back(value);
        else
            parameters.push_back(milaBuilder.CreateLoad(value));
        parametersIterator++;
    }
    
    milaBuilder.CreateCall(
            milaModule.getFunction(procedureNode->getIdentifier()),
            parameters
    );
    return nullptr;
}

llvm::Value* ASTWalker::visit(IdentifierNode *identifierNode) {
    std::string identifier = identifierNode->getIdentifier();
    return symbolTable[identifier];
}

llvm::Value* ASTWalker::visit(AssignNode *assignNode) {
    llvm::Value* value = assignNode->getValue()->walk(this);
    std::string identifier = assignNode->getIdentifier();
    milaBuilder.CreateStore(value, symbolTable[identifier]);
    return value;
}

llvm::Value* ASTWalker::visit(BinOpNode *binOpNode) {
    Value *L = loadValue(binOpNode->getLeftOperator());
    Value *R = loadValue(binOpNode->getRightOperator());

    if (!L || !R)
        return nullptr;

    std::string operationToken = binOpNode->getOperationToken();
    if(operationToken == "+")
        return milaBuilder.CreateAdd(L, R, "addtmp");
    else if(operationToken == "-")
        return milaBuilder.CreateSub(L, R, "subtmp");
    else if(operationToken == "*")
        return milaBuilder.CreateMul(L, R, "multmp");
    else if(operationToken == "mod")
        return milaBuilder.CreateURem(L, R, "uremtmp");
    else {
        std::cerr << "Invalid binary operator" << std::endl;
        return nullptr;
    }
}

llvm::AllocaInst* ASTWalker::createEntryBlockAlloca(llvm::Function *function, llvm::Type *ty,
                                                    const std::string &name) {
    IRBuilder<> TmpB(&function->getEntryBlock(),
                     function->getEntryBlock().begin());
    return TmpB.CreateAlloca(ty, 0, name);
}

llvm::Value* ASTWalker::loadValue(const std::string& identifier) {
    if(auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(symbolTable[identifier]))
        return milaBuilder.CreateLoad(alloca, identifier);
    else
        return symbolTable[identifier];
}

llvm::Value* ASTWalker::loadValue(ASTNode* node) {
    if(auto varNode = dynamic_cast<IdentifierNode*>(node))
        return loadValue(varNode->getIdentifier());
    else
        return node->walk(this);
}