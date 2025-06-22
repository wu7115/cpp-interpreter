#pragma once

#include <string>
#include "../token/token.h"

namespace lexer {

token::Token newToken(token::TokenType token, char ch);
bool isLetter(char ch);
bool isDigit(char ch);

class Lexer {
private:
    std::string input;
    int position = 0;
    int readPosition = 0;
    char ch = 0;

    void readChar();
    std::string readIdentifier();
    void skipWhitespace();
    std::string readNumber();
    char peekChar() const;

public:
    explicit Lexer(const std::string& input);

    token::Token nextToken();
};

}