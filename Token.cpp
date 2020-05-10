//
// Created by Marek Fořt on 2020-05-10.
//

#include "Token.h"

Token::Token(Kind kind, std::string value) :kind(kind), value(std::move(value)) {}

Kind Token::getKind() { return kind; }
std::string Token::getValue() { return value; }