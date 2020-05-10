//
// Created by Marek Fořt on 2020-05-10.
//

#include "IntNode.h"

IntNode::IntNode(int value) :value(value) {}

const void IntNode::print() {
    std::cout << "Integer node:" << std::endl;
    std::cout << "\tvalue: " << value << std::endl;
}