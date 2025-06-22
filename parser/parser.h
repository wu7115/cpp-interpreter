#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include "../lexer/lexer.h"
#include "../token/token.h"
#include "../ast/ast.h"

namespace parser {

using PrefixParseFn = std::function<std::shared_ptr<ast::Expression>()>;
using InfixParseFn = std::function<std::shared_ptr<ast::Expression>(std::shared_ptr<ast::Expression>)>;

enum Precedence {
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUCT,
    PREFIX,
    CALL,
};

class Parser {
public:
    std::vector<std::string> errors() const;
    std::shared_ptr<ast::Program> parseProgram();

    explicit Parser(std::shared_ptr<lexer::Lexer> l);
private:
    std::shared_ptr<lexer::Lexer> l;
    std::vector<std::string> errorMessages;
    token::Token curToken;
    token::Token peekToken;
    std::unordered_map<token::TokenType, PrefixParseFn> prefixParseFns;
    std::unordered_map<token::TokenType, InfixParseFn> infixParseFns;

    int peekPrecedence() const;
    int curPrecedence() const;

    std::shared_ptr<ast::Expression> parseIdentifier();
    std::shared_ptr<ast::Expression> parseBoolean();
    std::shared_ptr<ast::Expression> parseIntegerLiteral();
    std::shared_ptr<ast::Expression> parsePrefixExpression();
    std::shared_ptr<ast::Expression> parseInfixExpression(std::shared_ptr<ast::Expression> left);
    std::shared_ptr<ast::Expression> parseGroupedExpression();
    std::shared_ptr<ast::Expression> parseIfExpression();
    std::shared_ptr<ast::BlockStatement> parseBlockStatement();
    std::shared_ptr<ast::Expression> parseFunctionLiteral();
    std::vector<std::shared_ptr<ast::Identifier>> parseFunctionParameters();
    std::shared_ptr<ast::Expression> parseCallExpression(std::shared_ptr<ast::Expression> function);
    std::vector<std::shared_ptr<ast::Expression>> parseCallArguments();

    void peekError(token::TokenType t);
    void nextToken();

    std::shared_ptr<ast::Statement> parseStatement();
    std::shared_ptr<ast::LetStatement> parseLetStatement();
    std::shared_ptr<ast::ReturnStatement> parseReturnStatement();
    std::shared_ptr<ast::ExpressionStatement> parseExpressionStatement();
    std::shared_ptr<ast::Expression> parseExpression(int precedence);

    bool curTokenIs(token::TokenType t) const;
    bool peekTokenIs(token::TokenType t) const;
    bool expectPeek(token::TokenType t);

    void registerPrefix(token::TokenType tokenType, PrefixParseFn fn);
    void registerInfix(token::TokenType tokenType, InfixParseFn fn);
    void noPrefixParseFnError(token::TokenType tokenType);
};

}