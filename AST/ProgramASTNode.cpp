//
// Created by Marek Fořt on 2020-05-10.
//

#include <iostream>
#include "ProgramASTNode.h"

ProgramASTNode::ProgramASTNode(std::string name) :name(std::move(name)) {}

void ProgramASTNode::print() {
    std::cout << "Program node:" << std::endl;
    std::cout << "\tname: " << name << std::endl;
}