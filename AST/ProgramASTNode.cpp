//
// Created by Marek Fořt on 2020-05-10.
//

#include <iostream>
#include "ProgramASTNode.h"

ProgramASTNode::ProgramASTNode(
        std::string name,
        ASTNode* declarations,
        ASTNode* main):
        name(std::move(name)),
        declarations(declarations),
        main(main)
{}

ProgramASTNode::ProgramASTNode(
        const ProgramASTNode &programNode
):
        declarations(programNode.declarations),
        main(programNode.main)
{}

const void ProgramASTNode::print() {
    std::cout << "Program node:" << std::endl;
    std::cout << "name: " << name << std::endl;
    declarations->print();
    main->print();
}

ASTNode* ProgramASTNode::getDeclarations() {
    return declarations;
}

ASTNode* ProgramASTNode::getMain() {
    return main;
}