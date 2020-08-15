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
#include "AST/FunctionCallNode.h"
#include "AST/IdentifierNode.h"
#include "AST/AssignNode.h"
#include "AST/BinOpNode.h"
#include "AST/IfElseNode.h"
#include "AST/ForNode.h"
#include "AST/WhileNode.h"
#include "AST/BreakNode.h"
#include "AST/FunctionDeclarationNode.h"
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

    // create dec function
    {
        std::vector<Type*> Ints(1, Type::getInt32PtrTy(milaContext));
        FunctionType *FT = FunctionType::get(
                milaBuilder.getVoidTy(),
                Ints,
                false
        );

        Function *F = Function::Create(
                FT,
                Function::ExternalLinkage,
                "dec",
                milaModule
        );

        F->args().begin()->setName("x");

        BasicBlock *BB = BasicBlock::Create(milaBuilder.getContext(), "entry", F);
        milaBuilder.SetInsertPoint(BB);

        llvm::Value* value = F->args().begin();

        llvm::Value* loadedValue = milaBuilder.CreateLoad(value, "x");
        llvm::Value* oneValue = llvm::ConstantInt::get(milaContext, llvm::APInt(32, 1));
        llvm::Value* subValue = milaBuilder.CreateSub(loadedValue, oneValue, "subtmp");
        milaBuilder.CreateStore(subValue, value);

        milaBuilder.CreateRet(nullptr);
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

llvm::Function* ASTWalker::visit(FunctionDeclarationNode *functionNode) {
    llvm::Type *returnType = nullptr;
    switch(functionNode->getReturnType()) {
        case MilaType::integer:
            returnType = milaBuilder.getInt32Ty();
            break;
        case MilaType::void_type:
            returnType = milaBuilder.getVoidTy();
            break;
    }

    std::vector<llvm::Type*> paramaterTypes;
    std::vector<std::string> parameterNames;

    for(auto const& parameterPair: functionNode->getParameters()) {
        switch(parameterPair.second) {
            case MilaType::integer:
                paramaterTypes.push_back(milaBuilder.getInt32Ty());
                break;
            case MilaType::void_type:
                std::cerr << "Unexpected void value" << std::endl;
                return nullptr;
        }

        parameterNames.push_back(parameterPair.first);
    }

    FunctionType *FT = FunctionType::get(
            returnType,
            paramaterTypes,
            false
    );

    Function *F = Function::Create(
            FT,
            Function::ExternalLinkage,
            functionNode->getIdentifier(),
            milaModule
    );

    if (F->getName() != functionNode->getIdentifier()) {
        // Delete the one we just made and get the existing one.
        F->eraseFromParent();
        F = milaModule.getFunction(functionNode->getIdentifier());

        // If F already has a body, reject this.
        if (!F->empty()) {
            std::cerr << "redefinition of function" << std::endl;
            return nullptr;
        }

        // If F took a different number of args, reject.
        if (F->arg_size() != parameterNames.size()) {
            std::cerr << "redefinition of function with different # args" << std::endl;
            return nullptr;
        }
    }

    unsigned index = 0;
    for (Function::arg_iterator parameterIterator = F->arg_begin(); index != parameterNames.size();
         ++parameterIterator, ++index) {
        parameterIterator->setName(parameterNames[index]);

        // Add arguments to variable symbol table.
        symbolTable[parameterNames[index]] = parameterIterator;
    }

    // Create a new basic block to start insertion into.
    BasicBlock *BB = BasicBlock::Create(milaBuilder.getContext(), "entry", F);
    milaBuilder.SetInsertPoint(BB);

    // Enable to write into parameters
    for(auto const& parameterName: parameterNames) {
        llvm::Value* value = symbolTable[parameterName];
        AllocaInst* alloca = ASTWalker::createEntryBlockAlloca(F, Type::getInt32Ty(milaContext), parameterName);
        symbolTable[parameterName] = alloca;
        milaBuilder.CreateStore(value, alloca);
    }

    // Allocate space for return value if it's not void
    if(functionNode->getReturnType() != MilaType::void_type) {
        Function *function = milaBuilder.GetInsertBlock()->getParent();
        std::string identifier = functionNode->getIdentifier();
        AllocaInst* alloca = ASTWalker::createEntryBlockAlloca(function, returnType, identifier);
        symbolTable[identifier] = alloca;

        functionNode->getBlock()->walk(this);

        milaBuilder.CreateRet(milaBuilder.CreateLoad(alloca, identifier));
    } else {
        functionNode->getBlock()->walk(this);

        milaBuilder.CreateRet(nullptr);
    }

    verifyFunction(*F);

    return F;
}

llvm::Value* ASTWalker::visit(FunctionCallNode *procedureNode) {
    std::vector<Value*> parameters = {};
    Function* function = milaModule.getFunction(procedureNode->getIdentifier());
    auto parametersIterator = procedureNode->getParameters().begin();
    for(auto const& arg: function->args()) {
        if(arg.getType()->isPointerTy())
            parameters.push_back((*parametersIterator)->walk(this));
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
    std::string identifier = assignNode->getIdentifier();
    llvm::Value* value = loadValue(assignNode->getValue());
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
    else if(operationToken == "div")
        return milaBuilder.CreateSDiv(L, R, "sdivtmp");
    else if(operationToken == "mod")
        return milaBuilder.CreateURem(L, R, "uremtmp");
    else if(operationToken == "=")
        return milaBuilder.CreateICmpEQ(L, R, "cmptmp");
    else if(operationToken == "<>")
        return milaBuilder.CreateICmpNE(L, R, "notcmptmp");
    else if(operationToken == ">=")
        return milaBuilder.CreateICmpSGE(L, R, "sgetmp");
    else if(operationToken == "<=")
        return milaBuilder.CreateICmpSLE(L, R, "sletmp");
    else if(operationToken == ">")
        return milaBuilder.CreateICmpSGT(L, R, "sgttmp");
    else if(operationToken == "<")
        return milaBuilder.CreateICmpSLT(L, R, "slttmp");
    else {
        std::cerr << "Invalid binary operator" << std::endl;
        return nullptr;
    }

//    ICMP_EQ    = 32,  ///< equal
//    ICMP_NE    = 33,  ///< not equal
//    ICMP_UGT   = 34,  ///< unsigned greater than
//    ICMP_UGE   = 35,  ///< unsigned greater or equal
//    ICMP_ULT   = 36,  ///< unsigned less than
//    ICMP_ULE   = 37,  ///< unsigned less or equal
//    ICMP_SGT   = 38,  ///< signed greater than
//    ICMP_SGE   = 39,  ///< signed greater or equal
//    ICMP_SLT   = 40,  ///< signed less than
//    ICMP_SLE   = 41,  ///< signed less or equal
}

llvm::Value* ASTWalker::visit(WhileNode* whileNode) {
    // Make the new basic block for the loop header, inserting after current
    // block.
    Function *function = milaBuilder.GetInsertBlock()->getParent();
    BasicBlock *LoopBB = BasicBlock::Create(milaContext, "loop", function);

    // Insert an explicit fall through from the current block to the LoopBB.
    milaBuilder.CreateBr(LoopBB);

    // Start insertion in LoopBB.
    milaBuilder.SetInsertPoint(LoopBB);

    // Compute the end condition.

    Value* condition = whileNode->getCondition()->walk(this);

    Value *one = ConstantInt::get(Type::getInt32Ty(milaContext), 1);

    // Create the "after loop" block and insert it.
    BasicBlock *AfterBB = BasicBlock::Create(milaContext, "afterloop", function);
    loops.push(AfterBB);

    whileNode->getBody()->walk(this);

    // Insert the conditional branch into the end of LoopEndBB.
    milaBuilder.CreateCondBr(condition, LoopBB, AfterBB);

    // Any new code will be inserted in AfterBB.
    milaBuilder.SetInsertPoint(AfterBB);
    loops.pop();

    // for expr always returns 0.0.
    return Constant::getNullValue(Type::getInt32Ty(milaContext));
}

llvm::Value* ASTWalker::visit(ForNode *forNode) {
    llvm::Value* startValue = loadValue(forNode->getStart());
    // Make the new basic block for the loop header, inserting after current
    // block.
    Function *function = milaBuilder.GetInsertBlock()->getParent();
    BasicBlock *preheaderBB = milaBuilder.GetInsertBlock();
    BasicBlock *LoopBB = BasicBlock::Create(milaContext, "loop", function);

    // Insert an explicit fall through from the current block to the LoopBB.
    milaBuilder.CreateBr(LoopBB);

    // Start insertion in LoopBB.
    milaBuilder.SetInsertPoint(LoopBB);

    std::string valueName = forNode->getIdentifier();

    // Start the PHI node with an entry for Start.
    PHINode *variable = milaBuilder.CreatePHI(
            Type::getInt32Ty(milaContext),
            2,
            valueName
    );
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

    Value *nextVar;
    switch (forNode->getType()) {
        case ForType::to:
            nextVar = milaBuilder.CreateAdd(variable, one, "nextvar");
            break;
        case ForType::downto:
            nextVar = milaBuilder.CreateSub(variable, one, "nextvar");
    }

    // Compute the end condition.
    Value* endCondition = forNode->getEnd()->walk(this);

    endCondition = milaBuilder.CreateICmpNE(endCondition, variable, "loopcond");

    // Create the "after loop" block and insert it.
    BasicBlock *LoopEndBB = milaBuilder.GetInsertBlock();
    BasicBlock *AfterBB = BasicBlock::Create(milaContext, "afterloop", function);
    loops.push(AfterBB);

    // Insert the conditional branch into the end of LoopEndBB.
    milaBuilder.CreateCondBr(endCondition, LoopBB, AfterBB);

    // Any new code will be inserted in AfterBB.
    milaBuilder.SetInsertPoint(AfterBB);
    loops.pop();

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

    if(!llvm::dyn_cast<llvm::ReturnInst>(ifValue))
        milaBuilder.CreateBr(mergeBB);

    // Emit else block.
    function->getBasicBlockList().push_back(elseBB);
    milaBuilder.SetInsertPoint(elseBB);

    llvm::Value* elseValue;
    if(ifElseNode->getElseNode()) {
        elseValue = loadValue(ifElseNode->getElseNode());
    } else {
        elseValue = Constant::getNullValue(Type::getInt32Ty(milaContext));
    }

    if(!llvm::dyn_cast<llvm::ReturnInst>(elseValue))
        milaBuilder.CreateBr(mergeBB);

    // Emit merge block.
    function->getBasicBlockList().push_back(mergeBB);
    milaBuilder.SetInsertPoint(mergeBB);

    return Constant::getNullValue(Type::getInt32Ty(milaContext));
}

llvm::Value* ASTWalker::visit(BreakNode *breakNode) {
    llvm::BasicBlock* currentLoop = loops.front();
    loops.pop();
    return milaBuilder.CreateBr(currentLoop);
}

llvm::Value* ASTWalker::visit(ExitNode *exitNode) {
    return milaBuilder.CreateRetVoid();

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