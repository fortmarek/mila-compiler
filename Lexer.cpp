#include "Lexer.hpp"
#include <regex>

Token Lexer::firstToken() {
    char inputChar;
    while(std::cin >> std::noskipws >> inputChar)
        input.push_back(inputChar);
    currentPtr = input.begin();
    return lexToken();
}

Lexer::Lexer() :nextToken(firstToken()) {}

char Lexer::peek() { return *currentPtr; }

char Lexer::get() { return *currentPtr++; }

bool Lexer::isSpace(char character) {
    return character == ' ' || character == '\n';
}

bool Lexer::isDigit(char character) {
    std::regex re("[0-9]");
    std::cmatch m;
    return std::regex_search(&character, m, re);
}

bool Lexer::isIdentifier(char character) {
    std::regex re("[a-zA-Z]");
    std::cmatch m;
    return std::regex_search(&character, m, re);
}

Token Lexer::token(const std::string &str) {
    const std::map<std::string, Kind> kindMap = {
            {"program", Kind::tok_program},
            {"const", Kind::tok_const},
            {"var", Kind::tok_var}
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

Token Lexer::digitToken() {
    auto startOfToken = currentPtr;
    get();
    while(isDigit(peek()))
        get();
    std::string identifier(startOfToken, currentPtr);
    return Token(Kind::tok_integer, identifier);
}

Token Lexer::lexToken() {
    while (isSpace(peek()))
        get();
    if(isIdentifier(peek()))
        return identifierToken();
    if(isDigit(peek()))
        return digitToken();
    switch(get()) {
        case ';':
            return Token(Kind::tok_div, ";");
        case '=':
            return Token(Kind::tok_init, "=");
    }
    return Token(Kind::tok_end, "");
}

Token Lexer::getToken() {
    Token currentToken = nextToken;
    nextToken = lexToken();
    std::cout << nextToken.getKind() << std::endl;
    return currentToken;
}

Token Lexer::peekNextToken() {
    return nextToken;
}

