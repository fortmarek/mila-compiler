//
// Created by Marek Fořt on 2020-06-14.
//

#include "ForNode.h"

const void ForNode::print() {
    std::cout << "For Node" << std::endl;
    std::cout << "Start" << std::endl;
    start->print();
    std::cout << "End" << std::endl;
    end->print();
    std::cout << "Body" << std::endl;
    body->print();
}

ForNode::ForNode(std::string identifier, ASTNode *start, ASTNode *end, ASTNode *body) :identifier(identifier), start(start), end(end), body(body) {}

std::string ForNode::getIdentifier() {
    return identifier;
}

ASTNode* ForNode::getStart() {
    return start;
}

ASTNode* ForNode::getEnd() {
    return end;
}

ASTNode* ForNode::getBody() {
    return body;
}