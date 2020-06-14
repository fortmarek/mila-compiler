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
#include "AST/IfElseNode.h"
#include "AST/ForNode.h"
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

    visit(dynamic_cast<CompoundNode*>(mainNode));

    // return 0
    milaBuilder.CreateRet(ConstantInt::get(Type::getInt32Ty(milaContext), 0));
    return nullptr;
}

llvm::Value* ASTWalker::visit(CompoundNode *compound) {
    llvm::Value* value;
    for(auto const& child: compound->getChildren())
        value = child->walk(this);
    return value;
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
            parameters.push_back(loadValue(*parametersIterator));
        parametersIterator++;
    }
    
    Value* value = milaBuilder.CreateCall(
            milaModule.getFunction(procedureNode->getIdentifier()),
            parameters
    );
    return value;
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
    else if(operationToken == "=") {
        return milaBuilder.CreateICmp(llvm::CmpInst::ICMP_EQ, L, R, "cmptmp");
        // Convert bool 0/1 to double 0.0 or 1.0
//        return milaBuilder.CreateUIToFP(L, Type::getInt32Ty(milaContext), "booltmp");
    }
    else {
        std::cerr << "Invalid binary operator" << std::endl;
        return nullptr;
    }
}

llvm::Value* ASTWalker::visit(ForNode *forNode) {
    llvm::Value* startValue = loadValue(forNode->getStart());
    // Make the new basic block for the loop header, inserting after current
    // block.
    Function *function = milaBuilder.GetInsertBlock()->getParent();
    BasicBlock *preheaderBB = milaBuilder.GetInsertBlock();
    BasicBlock *LoopBB =
            BasicBlock::Create(milaContext, "loop", function);

    // Insert an explicit fall through from the current block to the LoopBB.
    milaBuilder.CreateBr(LoopBB);

    // Start insertion in LoopBB.
    milaBuilder.SetInsertPoint(LoopBB);

    std::string valueName = forNode->getIdentifier();

    // Start the PHI node with an entry for Start.
    PHINode *variable = milaBuilder.CreatePHI(Type::getInt32Ty(milaContext),
                                          2, valueName);
    variable->addIncoming(startValue, preheaderBB);

    // Within the loop, the variable is defined equal to the PHI node.  If it
    // shadows an existing variable, we have to restore it, so save it now.
    Value *oldValue = symbolTable[valueName];
    symbolTable[valueName] = variable;

    // Emit the body of the loop.  This, like any other expr, can change the
    // current BB.  Note that we ignore the value computed by the body, but don't
    // allow an error.
    forNode->getBody()->walk(this);

    Value *one = ConstantInt::get(Type::getInt32Ty(milaContext), 1);

    Value *nextVar = milaBuilder.CreateAdd(variable, one, "nextvar");

    // Compute the end condition.
    Value* endCondition = forNode->getEnd()->walk(this);

    endCondition = milaBuilder.CreateICmpNE(endCondition, variable, "loopcond");

    // Create the "after loop" block and insert it.
    BasicBlock *LoopEndBB = milaBuilder.GetInsertBlock();
    BasicBlock *AfterBB =
            BasicBlock::Create(milaContext, "afterloop", function);

    // Insert the conditional branch into the end of LoopEndBB.
    milaBuilder.CreateCondBr(endCondition, LoopBB, AfterBB);

    // Any new code will be inserted in AfterBB.
    milaBuilder.SetInsertPoint(AfterBB);

    // Add a new entry to the PHI node for the backedge.
    variable->addIncoming(nextVar, LoopEndBB);

    // TODO: Restore shadowed variable
    // Restore the unshadowed variable.
//    if (oldValue)
//        symbolTable[oldValue] = oldValue;
//    else
//        NamedValues.erase(VarName);

    // for expr always returns 0.0.
    return Constant::getNullValue(Type::getInt32Ty(milaContext));
}

llvm::Value* ASTWalker::visit(IfElseNode *ifElseNode) {
    llvm::Value* conditionValue = loadValue(ifElseNode->getConditionNode());

    Function *function = milaBuilder.GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at the
    // end of the function.
    BasicBlock *ifBB = BasicBlock::Create(milaContext, "then", function);
    BasicBlock *elseBB = BasicBlock::Create(milaContext, "else");
    BasicBlock *mergeBB = BasicBlock::Create(milaContext, "ifcont");

    milaBuilder.CreateCondBr(conditionValue, ifBB, elseBB);

    // Emit then value.
    milaBuilder.SetInsertPoint(ifBB);
    llvm::Value* ifValue = loadValue(ifElseNode->getIfNode());

    milaBuilder.CreateBr(mergeBB);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    ifBB = milaBuilder.GetInsertBlock();

    // Emit else block.
    function->getBasicBlockList().push_back(elseBB);
    milaBuilder.SetInsertPoint(elseBB);

    llvm::Value* elseValue = loadValue(ifElseNode->getElseNode());

    milaBuilder.CreateBr(mergeBB);
    // codegen of 'Else' can change the current block, update ElseBB for the PHI.
    elseBB = milaBuilder.GetInsertBlock();

    // Emit merge block.
    function->getBasicBlockList().push_back(mergeBB);
    milaBuilder.SetInsertPoint(mergeBB);
//    return milaBuilder.getInt32(0);
    llvm::PHINode *PN = milaBuilder.CreatePHI(Type::getInt32Ty(milaContext), 2, "iftmp");
    PN->addIncoming(ifValue, ifBB);
    PN->addIncoming(elseValue, elseBB);
    return PN;
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
    else {
        return node->walk(this);
    }
}