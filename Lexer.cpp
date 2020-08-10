#include "Lexer.hpp"
#include <regex>

Token Lexer::firstToken() {
    char inputChar;
    while(std::cin >> std::noskipws >> inputChar) {
//        std::cout << inputChar << std::endl;
        input.push_back(inputChar);
    }

    currentPtr = input.begin();
    return lexToken();
}

Lexer::Lexer() :nextToken(firstToken()) {}

std::string Lexer::peek() { return std::string (currentPtr, currentPtr + 1); }

std::string Lexer::get() { return std::string (currentPtr, currentPtr++); }

bool Lexer::isSpace(std::string character) {
    return character == " " || character == "\n" || character == "\t";
}

bool Lexer::isDigit(std::string character) {
    std::regex re("[0-9]");
    return std::regex_search(character, re);
}

bool Lexer::isIdentifier(std::string character) {
    std::regex re("[a-zA-Z0-9]");
    return std::regex_match(character, re);
}

bool Lexer::isFirstIdentifierChar(std::string character) {
    std::regex re("[a-zA-Z]");
    return std::regex_match(character, re);
}

Token Lexer::token(const std::string &str) {
    const std::map<std::string, Kind> kindMap = {
            {"program", Kind::tok_program},
            {"function", Kind::tok_function},
            {"procedure", Kind::tok_procedure},
            {"const", Kind::tok_const},
            {"var", Kind::tok_var},
            {";", Kind::tok_divider},
            {"=", Kind::tok_init},
            {":=", Kind::tok_assign},
            {":", Kind::tok_type},
            {"begin", Kind::tok_begin},
            {"end", Kind::tok_end},
            {"(", Kind::tok_left_paren},
            {")", Kind::tok_right_paren},
            {"*", Kind::tok_multiplier},
            {"\\", Kind::tok_division},
            {"+", Kind::tok_plus},
            {"-", Kind::tok_minus},
            {".", Kind::tok_dot},
            {"mod", Kind::tok_mod},
            {"if", Kind::tok_if},
            {"then", Kind::tok_then},
            {"while", Kind::tok_while},
            {"do", Kind::tok_do},
            {"else", Kind::tok_else},
            {"for", Kind::tok_for},
            {"to", Kind::tok_to},
            {"downto", Kind::tok_downto},
            {"<>", Kind::tok_notequal},
            {">=", Kind::tok_greaterequal},
            {"=<", Kind::tok_lessequal},
            {"break", Kind::tok_break},
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
    while(isIdentifier(peek())) {
        get();
    }
    std::string identifier(startOfToken, currentPtr);
    return token(identifier);
}

Token Lexer::digitToken() {
    auto startOfToken = currentPtr;
    get();
    while(isDigit(peek()))
        get();
    std::string identifier(startOfToken, currentPtr);
    return Token(Kind::tok_number, identifier);
}

Token Lexer::operatorToken() {
    auto startOfToken = currentPtr;
    get();
    while(isOperator(peek()))
        get();
    std::string operatorString(startOfToken, currentPtr);
    return token(operatorString);
}

bool Lexer::isOperator(std::string character) {
    std::regex re("[:=|&+*()-.<>]");
    return std::regex_search(character, re);
}

Token Lexer::lexToken() {
    while (isSpace(peek()))
        get();

    if(isOperator(peek()))
        return operatorToken();

    if(isFirstIdentifierChar(peek()))
        return identifierToken();

    if(isDigit(peek()))
        return digitToken();

    if(peek() == ";") {
        get();
        return Token(Kind::tok_divider, ";");
    }

    return Token(Kind::tok_end, "");
}

Token Lexer::getToken() {
    Token currentToken = nextToken;
    nextToken = lexToken();
//    std::cout << currentToken.getValue() << std::endl;
//    std::cout << currentToken.getKind() << std::endl;
//    std::cout << nextToken.getValue() << std::endl;
//    std::cout << nextToken.getKind() << std::endl;
//    std::cout << "-----" << std::endl;
    return currentToken;
}

Token Lexer::peekNextToken() {
    return nextToken;
}

