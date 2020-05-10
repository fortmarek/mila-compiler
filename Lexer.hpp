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

    Token gettok();
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
    Token identifierToken();
    Token token(const std::string & str);

};

#endif //PJPPROJECT_LEXER_HPP
