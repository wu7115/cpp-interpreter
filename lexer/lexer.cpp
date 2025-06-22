#include "lexer.h"
#include <cctype>
#include <string>

namespace lexer {

Lexer::Lexer(const std::string& input): input(input) {
    readChar();
}

void Lexer::readChar() {
    if(readPosition >= input.size()) {
        ch = 0;
    } else {
        ch = input[readPosition];
    }
    position = readPosition;
    readPosition++;
}

char Lexer::peekChar() const {
    if(readPosition >= input.size()) {
        return 0;
    } 
    return input[readPosition];
}

void Lexer::skipWhitespace() {
    while(std::isspace(ch)) {
        readChar();
    }
}

std::string Lexer::readIdentifier() {
    int start = position;
    while(isLetter(ch)) {
        readChar();
    }
    return input.substr(start, position - start);
}

std::string Lexer::readNumber() {
    int start = position;
    while(isDigit(ch)) {
        readChar();
    }
    return input.substr(start, position - start);
}

token::Token Lexer::nextToken() {
    token::Token tok;
    skipWhitespace();

    switch(ch) {
        case '=':
            if(peekChar() == '=') {
                char c = ch;
                readChar();
                tok = token::Token{token::TokenType::EQ, std::string(1, c) + ch};
            } else {
                tok = newToken(token::TokenType::ASSIGN, ch);
            }
            break;
        case '+':
            tok = newToken(token::TokenType::PLUS, ch);
            break;
        case '-':
            tok = newToken(token::TokenType::MINUS, ch);
            break;
        case '!':
            if(peekChar() == '=') {
                char c = ch;
                readChar();
                tok = token::Token{token::TokenType::NOT_EQ, std::string(1, c) + ch};
            } else {
                tok = newToken(token::TokenType::BANG, ch);
            }
            break;
        case '/':
            tok = newToken(token::TokenType::SLASH, ch);
            break;
        case '*':
            tok = newToken(token::TokenType::ASTERISK, ch);
            break;
        case '<':
            tok = newToken(token::TokenType::LT, ch);
            break;
        case '>':
            tok = newToken(token::TokenType::GT, ch);
            break;
        case ';':
            tok = newToken(token::TokenType::SEMICOLON, ch);
            break;
        case '(':
            tok = newToken(token::TokenType::LPAREN, ch);
            break;
        case ')':
            tok = newToken(token::TokenType::RPAREN, ch);
            break;
        case ',':
            tok = newToken(token::TokenType::COMMA, ch);
            break;
        case '{':
            tok = newToken(token::TokenType::LBRACE, ch);
            break;
        case '}':
            tok = newToken(token::TokenType::RBRACE, ch);
            break;
        case 0:
            tok = {token::TokenType::EOF_TOKEN, ""};
            break;
        default:
            if(isLetter(ch)) {
                std::string literal = readIdentifier();
                token::TokenType type = token::lookupIdent(literal);
                return token::Token(type, literal);
            } else if(isDigit(ch)) {
                std::string literal = readNumber();
                return token::Token(token::TokenType::INT, literal);
            } else {
                tok = newToken(token::TokenType::ILLEGAL, ch);
            }
            break;
    }

    readChar();
    return tok;
}

token::Token newToken(token::TokenType type, char ch) {
    return token::Token{type, std::string(1, ch)};
}

bool isLetter(char ch) {
    return std::isalpha(ch) || ch == '_';
}

bool isDigit(char ch) {
    return std::isdigit(ch);
}

}