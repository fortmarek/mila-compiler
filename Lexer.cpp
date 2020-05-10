#include "Lexer.hpp"
#include <regex>

Lexer::Lexer() {
    char inputChar;
    while(std::cin >> std::noskipws >> inputChar)
        input.push_back(inputChar);
    currentPtr = input.begin();
}

char Lexer::peek() { return *currentPtr; }

char Lexer::get() { return *currentPtr++; }

bool Lexer::isSpace(char character) {
    return character == ' ' || character == '\n';
}

bool Lexer::isDigit(char character) {
    std::regex re("[0-9]");
    std::cmatch m;
    return std::regex_search(reinterpret_cast<const char*>(character), m, re);
}

bool Lexer::isIdentifier(char character) {
    std::regex re("[a-zA-Z]");
    std::cmatch m;
    return std::regex_search(&character, m, re);
}

Token Lexer::token(const std::string &str) {
    const std::map<std::string, Kind> kindMap = {
            {"program", Kind::tok_program}
    };

    auto token = kindMap.find(str);
    if(token != kindMap.end())
        return Token(token->second, str);
    else
        return Token(Kind::tok_identifier, str);
}

Token Lexer::identifierToken() {
    auto startOfToken = currentPtr;
    get();
    while(isIdentifier(peek()))
        get();
    std::string identifier(startOfToken, currentPtr);
    return token(identifier);
}

/*
 * Function to return the next token from standard input
 * the variable 'm_IdentifierStr' is set there in case of an identifier,
 * the variable 'm_NumVal' is set there in case of a number.
 */
Token Lexer::gettok() {
    while (isSpace(peek()))
        get();
    if(isIdentifier(peek()))
        return identifierToken();
    return Token(Kind::tok_end, "");
}

