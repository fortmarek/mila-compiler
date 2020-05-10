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
            {"var", Kind::tok_var},
            {";", Kind::tok_div},
            {"=", Kind::tok_init},
            {":=", Kind::tok_assign},
            {":", Kind::tok_type},
            {"begin", Kind::tok_begin},
            {"end", Kind::tok_end},
            {"(", Kind::tok_left_paren},
            {")", Kind::tok_right_paren},
            {"*", Kind::tok_multiplier},
            {"+", Kind::tok_plus},
            {"-", Kind::tok_minus},
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

Token Lexer::operatorToken() {
    auto startOfToken = currentPtr;
    get();
    while(isOperator(peek()))
        get();
    std::string operatorString(startOfToken, currentPtr);
    return token(operatorString);
}

bool Lexer::isOperator(char character) {
    std::regex re("[:=|&+-*()]");
    std::cmatch m;
    return std::regex_search(&character, m, re);
}

Token Lexer::lexToken() {
    while (isSpace(peek()))
        get();

    if(isOperator(peek()))
        return operatorToken();

    if(isIdentifier(peek()))
        return identifierToken();

    if(isDigit(peek()))
        return digitToken();

    if(peek() == ';') {
        get();
        return Token(Kind::tok_div, ";");
    }

    return Token(Kind::tok_end, "");
}

Token Lexer::getToken() {
    Token currentToken = nextToken;
    nextToken = lexToken();
    std::cout << currentToken.getValue() << std::endl;
    std::cout << currentToken.getKind() << std::endl;
    std::cout << nextToken.getValue() << std::endl;
    std::cout << nextToken.getKind() << std::endl;
    std::cout << "-----" << std::endl;
    return currentToken;
}

Token Lexer::peekNextToken() {
    return nextToken;
}

