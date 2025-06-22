#pragma once
#include <string>
#include <unordered_map>

namespace token {

enum class TokenType {
    ILLEGAL,
    EOF_TOKEN,

    IDENT,
    INT,

    ASSIGN,
    PLUS,
    MINUS,
    BANG,
    ASTERISK,
    SLASH,

    LT,
    GT,
    EQ,
    NOT_EQ,

    COMMA,
    SEMICOLON,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN,
};

struct Token {
    TokenType type;
    std::string literal;

    Token()
        : type(TokenType::ILLEGAL), literal("") {}
    Token(TokenType t, const std::string& lit): type(t), literal(lit) {}
};

inline TokenType lookupIdent(const std::string& ident) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"fn", TokenType::FUNCTION},
        {"let", TokenType::LET},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"return", TokenType::RETURN},
    };

    auto it = keywords.find(ident);
    if(it != keywords.end()) {
        return it->second;
    }
    return TokenType::IDENT;
}

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL: return "ILLEGAL";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::IDENT: return "IDENT";
        case TokenType::INT: return "INT";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::BANG: return "BANG";
        case TokenType::ASTERISK: return "ASTERISK";
        case TokenType::SLASH: return "SLASH";
        case TokenType::LT: return "LT";
        case TokenType::GT: return "GT";
        case TokenType::EQ: return "EQ";
        case TokenType::NOT_EQ: return "NOT_EQ";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::LET: return "LET";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::RETURN: return "RETURN";
        
        
        
        default: return "UNKNOWN";
    }
}

}