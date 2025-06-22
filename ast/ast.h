#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../token/token.h"

namespace ast {

// === Base AST Interface ===
class Node {
public:
    virtual std::string tokenLiteral() const = 0;
    virtual std::string toString() const = 0;
    virtual ~Node() {}
};

class Statement : public Node {
public:
    virtual void statementNode() = 0;
};

class Expression : public Node {
public:
    virtual void expressionNode() = 0;
};

// === Program (root node) ===
class Program : public Node {
public:
    std::vector<std::shared_ptr<Statement>> statements;

    std::string tokenLiteral() const override {
        if(statements.size() > 0) {
            return statements[0]->tokenLiteral(); // returns the first statement's token literal, e.g., "let x = 5;" => "let"
        } else {
            return "";
        }
    }

    std::string toString() const override {
        std::string result;

        for(const auto& s : statements) {
            result += s->toString();
        }
        return result;
    }
};

// === Identifier Expression ===
class Identifier : public Expression {
public:
    token::Token token;
    std::string value;

    Identifier(token::Token token, std::string value)
        : token(token), value(value) {}

    void expressionNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        return value;
    }
};

// === Let Statement ===
class LetStatement : public Statement {
public:
    token::Token token;
    std::shared_ptr<Identifier> name;
    std::shared_ptr<Expression> value;

    LetStatement(token::Token token, std::shared_ptr<Identifier> name, std::shared_ptr<Expression> value)
        : token(token), name(name), value(value) {}

    void statementNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        std::string result;

        result = tokenLiteral() + " " + name->toString() + " = ";
        if(value != nullptr) {
            result += value->toString();
        }
        result += ";";
        return result;
    }
};

// === Return Statement ===
class ReturnStatement : public Statement {
public:
    token::Token token;
    std::shared_ptr<Expression> returnValue;

    ReturnStatement(token::Token token, std::shared_ptr<Expression> returnValue)
        : token(token), returnValue(returnValue) {}
    
    void statementNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        std::string result;

        result += tokenLiteral() + " ";
        if(returnValue != nullptr) {
            result += returnValue->toString();
        }
        result += ";";
        return result;
    }
};

// === ExpressionStatement ===
class ExpressionStatement : public Statement {
public:
    token::Token token;
    std::shared_ptr<Expression> expression;

    ExpressionStatement(token::Token token, std::shared_ptr<Expression> expression)
        : token(token), expression(expression) {}
    
    void statementNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        if(expression != nullptr) {
            return expression -> toString();
        }
        return "";
    }
};

// === Integer Literal ===
class IntegerLiteral : public Expression {
public:
    token::Token token;
    int64_t value;

    IntegerLiteral(token::Token token, int value)
        : token(token), value(value) {}
    
    void expressionNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        return token.literal;
    }
};

// === Prefix Expression ===
class PrefixExpression : public Expression {
public:
    token::Token token;
    std::string op;
    std::shared_ptr<Expression> right;

    PrefixExpression(token::Token token, std::string op, std::shared_ptr<Expression> right)
        : token(token), op(op), right(right) {}
    
    void expressionNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        std::string result;

        result += "(" + op + right->toString() + ")";
        return result;
    }
};

// === Infix Expression ===
class InfixExpression : public Expression {
public:
    token::Token token;
    std::shared_ptr<Expression> left;
    std::string op;
    std::shared_ptr<Expression> right;

    InfixExpression(token::Token token, std::shared_ptr<Expression> left, std::string op, std::shared_ptr<Expression> right)
        : token(token), left(left), op(op), right(right) {}
    
    void expressionNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        std::string result;

        result += "(" + left -> toString() + " " + op + " " + right -> toString() + ")";
        return result;
    }
};

// === Boolean Expression ===
class Boolean : public Expression {
public:
    token::Token token;
    bool value;

    Boolean(token::Token token, bool value)
        : token(token), value(value) {}
    
    void expressionNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        return token.literal;
    }
};

// === Block Statement ===
class BlockStatement : public Statement {
public:
    token::Token token;
    std::vector<std::shared_ptr<Statement>> statements;

    BlockStatement(token::Token token, std::vector<std::shared_ptr<Statement>> statements)
        : token(token), statements(statements) {}
    
    void statementNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        std::string result;

        for(const auto& s : statements) {
            result += s -> toString();
        }
        return result;
    }
};

// === If Expression ===
class IfExpression : public Expression {
public:
    token::Token token;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<BlockStatement> consequence;
    std::shared_ptr<BlockStatement> alternative;

    IfExpression(token::Token token, std::shared_ptr<Expression> condition, std::shared_ptr<BlockStatement> consequence, std::shared_ptr<BlockStatement> alternative)
        : token(token), condition(condition), consequence(consequence), alternative(alternative) {}
    
    void expressionNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        std::string result;

        result += "if" + condition->toString() + " " + consequence->toString();
        if(alternative != nullptr) {
            result += "else" + alternative -> toString();
        }
        return result;
    }
};

// === Function Literal ===
class FunctionLiteral : public Expression {
public:
    token::Token token;
    std::vector<std::shared_ptr<Identifier>> parameters;
    std::shared_ptr<BlockStatement> body;

    FunctionLiteral(token::Token token, std::vector<std::shared_ptr<Identifier>> parameters, std::shared_ptr<BlockStatement> body)
        : token(token), parameters(parameters), body(body) {}
    
    void expressionNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }  

    std::string toString() const override {
        std::string result;

        std::vector<std::string> params;
        result += tokenLiteral() + "(";
        for(int i=0;i<parameters.size();i++) {
            result += parameters[i] -> toString();
            if(i < parameters.size() - 1) {
                result += ", ";
            }
        }
        result += ") " + body -> toString();
        return result;
    }
};

// === Call Expression ===
class CallExpression : public Expression {
public:
    token::Token token;
    std::shared_ptr<Expression> function;
    std::vector<std::shared_ptr<Expression>> arguments;

    CallExpression(token::Token token, std::shared_ptr<Expression> function, std::vector<std::shared_ptr<Expression>> arguments)
        : token(token), function(function), arguments(arguments) {}
    
    void expressionNode() override {}

    std::string tokenLiteral() const override {
        return token.literal;
    }

    std::string toString() const override {
        std::string result;

        result += function -> toString() + "(";
        for(int i=0;i<arguments.size();i++) {
            result += arguments[i] -> toString();
            if(i < arguments.size() - 1) {
                result += ", ";
            }
        }
        result += ")";
        return result;
    }
};

}