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
    std::string peek();
    std::string get();
    bool isSpace(std::string character);
    bool isDigit(std::string character);
    bool isFirstIdentifierChar(std::string character);
    bool isIdentifier(std::string character);
    bool isOperator(std::string character);
    Token operatorToken();
    Token identifierToken();
    Token digitToken();
    Token token(const std::string & str);
    Token nextToken;
    Token lexToken();
    Token firstToken();
};

#endif //PJPPROJECT_LEXER_HPP
