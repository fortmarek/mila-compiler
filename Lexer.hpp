#ifndef PJPPROJECT_LEXER_HPP
#define PJPPROJECT_LEXER_HPP

#include <vector>
#include <map>
#include <iostream>
#include "Token.h"

class Lexer {
public:
    Lexer();
    ~Lexer() = default;

    Token getToken();
    Token peekNextToken();
    const std::string& identifierStr() const { return this->m_IdentifierStr; }
    int numVal() { return this->m_NumVal; }
private:
    std::string m_IdentifierStr;
    int m_NumVal = 0;
    std::vector<char> input = {};
    std::vector<char>::iterator currentPtr;
    char peek();
    char get();
    bool isSpace(char character);
    bool isDigit(char character);
    bool isIdentifier(char character);
    bool isOperator(char character);
    Token operatorToken();
    Token identifierToken();
    Token digitToken();
    Token token(const std::string & str);
    Token nextToken;
    Token lexToken();
    Token firstToken();
};

#endif //PJPPROJECT_LEXER_HPP
