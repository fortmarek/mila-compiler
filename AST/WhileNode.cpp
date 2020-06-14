//
// Created by Marek Fořt on 2020-06-14.
//

#include "WhileNode.h"

WhileNode::WhileNode(ASTNode *condition, ASTNode *body) :condition(condition), body(body) {}

ASTNode* WhileNode::getBody() {
    return body;
}

ASTNode* WhileNode::getCondition() {
    return condition;
}

const void WhileNode::print() {
    std::cout << "While Node" << std::endl;
    std::cout << "Condition" << std::endl;
    condition->print();
    std::cout << "While body" << std::endl;
    body->print();
}