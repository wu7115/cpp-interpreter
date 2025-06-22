#include "parser.h"

namespace parser{
namespace {
std::unordered_map<token::TokenType, Precedence> precedences = {
    {token::TokenType::EQ, EQUALS},
    {token::TokenType::NOT_EQ, EQUALS},
    {token::TokenType::LT, LESSGREATER},
    {token::TokenType::GT, LESSGREATER},
    {token::TokenType::PLUS, SUM},
    {token::TokenType::MINUS, SUM},
    {token::TokenType::SLASH, PRODUCT},
    {token::TokenType::ASTERISK, PRODUCT},
    {token::TokenType::LPAREN, CALL},
};
}

Parser::Parser(std::shared_ptr<lexer::Lexer> l) : l(l) {
    Parser::nextToken();
    Parser::nextToken();

    registerPrefix(token::TokenType::IDENT, [this]() { return parseIdentifier(); });
    registerPrefix(token::TokenType::INT, [this]() { return parseIntegerLiteral(); });
    registerPrefix(token::TokenType::BANG, [this]() { return parsePrefixExpression(); });
    registerPrefix(token::TokenType::MINUS, [this]() { return parsePrefixExpression(); });
    registerPrefix(token::TokenType::TRUE, [this]() { return parseBoolean(); });
    registerPrefix(token::TokenType::FALSE, [this]() { return parseBoolean(); });
    registerPrefix(token::TokenType::LPAREN, [this]() { return parseGroupedExpression(); });
    registerPrefix(token::TokenType::IF, [this]() { return parseIfExpression(); });
    registerPrefix(token::TokenType::FUNCTION, [this]() { return parseFunctionLiteral(); });

    registerInfix(token::TokenType::PLUS, [this](std::shared_ptr<ast::Expression> left) { return parseInfixExpression(left); });
    registerInfix(token::TokenType::MINUS, [this](std::shared_ptr<ast::Expression> left) { return parseInfixExpression(left); });
    registerInfix(token::TokenType::ASTERISK, [this](std::shared_ptr<ast::Expression> left) { return parseInfixExpression(left); });
    registerInfix(token::TokenType::SLASH, [this](std::shared_ptr<ast::Expression> left) { return parseInfixExpression(left); });
    registerInfix(token::TokenType::EQ, [this](std::shared_ptr<ast::Expression> left) { return parseInfixExpression(left); });
    registerInfix(token::TokenType::NOT_EQ, [this](std::shared_ptr<ast::Expression> left) { return parseInfixExpression(left); });
    registerInfix(token::TokenType::LT, [this](std::shared_ptr<ast::Expression> left) { return parseInfixExpression(left); });
    registerInfix(token::TokenType::GT, [this](std::shared_ptr<ast::Expression> left) { return parseInfixExpression(left); });
    registerInfix(token::TokenType::LPAREN, [this](std::shared_ptr<ast::Expression> left) { return parseCallExpression(left); });
}

int Parser::peekPrecedence() const {
    auto it = precedences.find(peekToken.type);
    return (it != precedences.end()) ? it->second : LOWEST;
}

int Parser::curPrecedence() const {
    auto it = precedences.find(curToken.type);
    return (it != precedences.end()) ? it->second : LOWEST;
}

std::shared_ptr<ast::Expression> Parser::parseIdentifier() {
    return std::make_shared<ast::Identifier>(curToken, curToken.literal);
}

std::shared_ptr<ast::Expression> Parser::parseBoolean() {
    return std::make_shared<ast::Boolean>(curToken, curTokenIs(token::TokenType::TRUE));
}

std::shared_ptr<ast::Expression> Parser::parseIntegerLiteral() {
    try {
        int64_t value = std::stoll(curToken.literal);
        return std::make_shared<ast::IntegerLiteral>(curToken, value);
    } catch (...) {
        errorMessages.push_back("could not parse " + curToken.literal + " as integer");
        return nullptr;
    }
}

std::shared_ptr<ast::Expression> Parser::parsePrefixExpression() {
    auto expression = std::make_shared<ast::PrefixExpression>(curToken, curToken.literal, nullptr);
    nextToken();
    expression->right = parseExpression(PREFIX);
    return expression;
}

std::shared_ptr<ast::Expression> Parser::parseInfixExpression(std::shared_ptr<ast::Expression> left) {
    auto expression = std::make_shared<ast::InfixExpression>(curToken, left, curToken.literal, nullptr);
    int precedence = curPrecedence();
    nextToken();
    expression->right = parseExpression(precedence);
    return expression;
}

std::shared_ptr<ast::Expression> Parser::parseGroupedExpression() {
    nextToken();
    auto exp = parseExpression(LOWEST);
    if (!expectPeek(token::TokenType::RPAREN)) {
        return nullptr;
    }
    return exp;
}

std::shared_ptr<ast::Expression> Parser::parseIfExpression() {
    auto expression = std::make_shared<ast::IfExpression>(curToken, nullptr, nullptr, nullptr);
    if (!expectPeek(token::TokenType::LPAREN)) {
        return nullptr;
    }
    nextToken();
    expression->condition = parseExpression(LOWEST);

    if (!expectPeek(token::TokenType::RPAREN)) {
        return nullptr;
    }
    if (!expectPeek(token::TokenType::LBRACE)) {
        return nullptr;
    }

    expression->consequence = parseBlockStatement();
    if (peekTokenIs(token::TokenType::ELSE)) {
        nextToken();
        if (!expectPeek(token::TokenType::LBRACE)) {
            return nullptr;
        }
        expression->alternative = parseBlockStatement();
    }
    return expression;
}

std::shared_ptr<ast::BlockStatement> Parser::parseBlockStatement() {
    auto block = std::make_shared<ast::BlockStatement>(curToken, std::vector<std::shared_ptr<ast::Statement>>());
    nextToken();

    while(!curTokenIs(token::TokenType::RBRACE) && !curTokenIs(token::TokenType::EOF_TOKEN)) {
        auto stmt = parseStatement();
        if (stmt != nullptr) {
            block->statements.push_back(stmt);
        }
        nextToken();
    }
    return block;
}

std::shared_ptr<ast::Expression> Parser::parseFunctionLiteral() {
    auto lit = std::make_shared<ast::FunctionLiteral>(curToken, std::vector<std::shared_ptr<ast::Identifier>>(), nullptr);
    if (!expectPeek(token::TokenType::LPAREN)) {
        return nullptr;
    }

    lit->parameters = parseFunctionParameters();
    if (!expectPeek(token::TokenType::LBRACE)) {
        return nullptr;
    }

    lit->body = parseBlockStatement();
    return lit;
}

std::vector<std::shared_ptr<ast::Identifier>> Parser::parseFunctionParameters() {
    std::vector<std::shared_ptr<ast::Identifier>> identifiers;
    if (peekTokenIs(token::TokenType::RPAREN)) {
        nextToken();
        return identifiers;
    }
    nextToken();

    auto ident = std::make_shared<ast::Identifier>(curToken, curToken.literal);
    identifiers.push_back(ident);

    while(peekTokenIs(token::TokenType::COMMA)) {
        nextToken();
        nextToken();
        ident = std::make_shared<ast::Identifier>(curToken, curToken.literal);
        identifiers.push_back(ident);
    }

    if (!expectPeek(token::TokenType::RPAREN)) {
        return {};
    }
    return identifiers;
}

std::shared_ptr<ast::Expression> Parser::parseCallExpression(std::shared_ptr<ast::Expression> function) {
    auto exp = std::make_shared<ast::CallExpression>(curToken, function, std::vector<std::shared_ptr<ast::Expression>>());
    exp->arguments = parseCallArguments();
    return exp;
}

std::vector<std::shared_ptr<ast::Expression>> Parser::parseCallArguments() {
    std::vector<std::shared_ptr<ast::Expression>> args;
    if (peekTokenIs(token::TokenType::RPAREN)) {
        nextToken();
        return args;
    }

    nextToken();
    args.push_back(parseExpression(LOWEST));

    while(peekTokenIs(token::TokenType::COMMA)) {
        nextToken();
        nextToken();
        args.push_back(parseExpression(LOWEST));
    }

    if (!expectPeek(token::TokenType::RPAREN)) {
        return {};
    }
    return args;
}

std::vector<std::string> Parser::errors() const{
    return errorMessages;
}

void Parser::peekError(token::TokenType t) {
    std::string msg = "expected next token to be " + token::tokenTypeToString(t) + ", got " + token::tokenTypeToString(peekToken.type) + " instead.";
    errorMessages.push_back(msg);
}

void Parser::nextToken() {
    curToken = peekToken;
    peekToken = l -> nextToken();
}

std::shared_ptr<ast::Program> Parser::parseProgram() {
    auto program = std::make_shared<ast::Program>();
    
    while (curToken.type != token::TokenType::EOF_TOKEN) {
        auto stmt = parseStatement();
        if (stmt != nullptr) {
            program->statements.push_back(stmt);
        }
        nextToken();
    }
    return program;
}

std::shared_ptr<ast::Statement> Parser::parseStatement() {
    if (curToken.type == token::TokenType::LET) {
        return parseLetStatement();
    } else if (curToken.type == token::TokenType::RETURN) {
        return parseReturnStatement();
    } else {
        return parseExpressionStatement();
    }
}

std::shared_ptr<ast::LetStatement> Parser::parseLetStatement() {
    auto stmt = std::make_shared<ast::LetStatement>(curToken, nullptr, nullptr);
    if (!expectPeek(token::TokenType::IDENT)) {
        return nullptr;
    }
    stmt->name = std::make_shared<ast::Identifier>(curToken, curToken.literal);
    if (!expectPeek(token::TokenType::ASSIGN)) {
        return nullptr;
    }
    nextToken();
    stmt->value = parseExpression(LOWEST);
    while (!curTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }

    return stmt;
}

std::shared_ptr<ast::ReturnStatement> Parser::parseReturnStatement() {
    auto stmt = std::make_shared<ast::ReturnStatement>(curToken, nullptr);
    nextToken();
    stmt->returnValue = parseExpression(LOWEST);
    while (!curTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }
    return stmt;
}

std::shared_ptr<ast::ExpressionStatement> Parser::parseExpressionStatement() {
    auto stmt = std::make_shared<ast::ExpressionStatement>(curToken, nullptr);
    stmt->expression = parseExpression(LOWEST);
    if (peekTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }
    return stmt;
}

std::shared_ptr<ast::Expression> Parser::parseExpression(int precedence) {
    auto prefix = prefixParseFns.find(curToken.type);
    if (prefix == prefixParseFns.end()) {
        noPrefixParseFnError(curToken.type);
        return nullptr;
    }
    auto leftExp = prefix->second();

    while (!peekTokenIs(token::TokenType::SEMICOLON) && precedence < peekPrecedence()) {
        auto infix = infixParseFns.find(peekToken.type);
        if (infix == infixParseFns.end()) {
            return leftExp;
        }
        nextToken();
        leftExp = infix->second(leftExp);
    }
    return leftExp;
}

bool Parser::curTokenIs(token::TokenType t) const {
    return curToken.type == t;
}

bool Parser::peekTokenIs(token::TokenType t) const {
    return peekToken.type == t;
}

bool Parser::expectPeek(token::TokenType t) {
    if (peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

void Parser::registerPrefix(token::TokenType tokenType, PrefixParseFn fn) {
    prefixParseFns[tokenType] = fn;
}

void Parser::registerInfix(token::TokenType tokenType, InfixParseFn fn) {
    infixParseFns[tokenType] = fn;
}

void Parser::noPrefixParseFnError(token::TokenType t) {
    std::string msg = "no prefix parse function found for " + token::tokenTypeToString(t);
    errorMessages.push_back(msg);
}

}