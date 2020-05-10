#include "Lexer.hpp"
#include <regex>

Lexer::Lexer() {
    char inputChar;
    while(std::cin >> std::noskipws >> inputChar)
        input.push_back(inputChar);
    currentPtr = input.begin();
    std::cout << *currentPtr << std::endl;
}

char Lexer::peek() { return *currentPtr; }

char Lexer::get() { return *currentPtr++; }

bool Lexer::is_space(char character) {
    return character == ' ' || character == '\n';
}

bool Lexer::is_digit(char character) {
    std::regex re("[0-9]");
    std::cmatch m;
    return std::regex_search(reinterpret_cast<const char*>(character), m, re);
}

bool Lexer::is_identifier(char character) {
    std::regex re("[a-zA-Z]");
    std::cmatch m;
    std::cout << character << std::endl;
    std::cout << std::regex_search(&character, m, re) << std::endl;
    return std::regex_search(&character, m, re);
}

Token Lexer::token(const std::string &str) {
    const std::map<std::string, Token> tokenMap = {
            {"program", Token::tok_program}
    };

    auto token = tokenMap.find(str);
    if(token != tokenMap.end())
        return token->second;
    else
        return Token::tok_identifier;
}

int Lexer::identifier_token() {
    auto startOfToken = currentPtr;
    get();
    while(is_identifier(peek()))
        get();
    std::string identifier(startOfToken, currentPtr);
    std::cout << identifier << std::endl;
    return token(identifier);
}

/*
 * Function to return the next token from standard input
 * the variable 'm_IdentifierStr' is set there in case of an identifier,
 * the variable 'm_NumVal' is set there in case of a number.
 */
int Lexer::gettok() {
    while (is_space(peek()))
        get();
    if(is_identifier(peek()))
        return identifier_token();
    return tok_end;
}

