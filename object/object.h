#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../ast/ast.h"

namespace object {

using ObectType = std::string;

enum class ObjectType{
    INTEGER_OBJ,
    BOOLEAN_OBJ,
    NULL_OBJ,
    RETURN_VALUE_OBJ,
    ERROR_OBJ,
    FUNCTION_OBJ
};

inline std::string objectTypeToString(ObjectType type) {
    switch (type) {
        case ObjectType::INTEGER_OBJ: return "INTEGER";
        case ObjectType::BOOLEAN_OBJ: return "BOOLEAN";
        case ObjectType::NULL_OBJ: return "NULL";
        case ObjectType::RETURN_VALUE_OBJ: return "RETURN_VALUE";
        case ObjectType::ERROR_OBJ: return "ERROR";
        case ObjectType::FUNCTION_OBJ: return "FUNCTION";
    }
}

class Object {
public:
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
    virtual ~Object() {}
};

class Integer : public Object {
public:
    int64_t value;
    Integer(int64_t value) : value(value) {}

    ObjectType type() const override { return ObjectType::INTEGER_OBJ; }
    std::string inspect() const override { return std::to_string(value); }
};

class Boolean : public Object {
public:
    bool value;
    Boolean(bool value) : value(value) {}

    ObjectType type() const override { return ObjectType::BOOLEAN_OBJ; }
    std::string inspect() const override { return value ? "true" : "false"; }
};

class Null : public Object {
public:
    ObjectType type() const override { return ObjectType::NULL_OBJ; }
    std::string inspect() const override { return "null"; }
};

class ReturnValue : public Object {
public:
    std::shared_ptr<Object> value;
    ReturnValue(std::shared_ptr<Object> value) : value(value) {}

    ObjectType type() const override { return ObjectType::RETURN_VALUE_OBJ; }
    std::string inspect() const override { return value->inspect(); }
};

class Error : public Object {
public:
    std::string message;
    Error(std::string message) : message(message) {}

    ObjectType type() const override { return ObjectType::ERROR_OBJ; }
    std::string inspect() const override { return "ERROR: " + message; }
};

class Environment;

class Function : public Object {
public:
    std::vector<std::shared_ptr<ast::Identifier>> parameters;
    std::shared_ptr<ast::BlockStatement> body;
    std::shared_ptr<Environment> env;
    Function(std::vector<std::shared_ptr<ast::Identifier>> parameters, std::shared_ptr<ast::BlockStatement> body, std::shared_ptr<Environment> env)
        : parameters(parameters), body(body), env(env) {}
    
    ObjectType type() const override { return ObjectType::FUNCTION_OBJ; }
    std::string inspect() const override {
        std::string result = "fn(";

        for (size_t i=0; i<parameters.size(); i++) {
            result += parameters[i]->toString();
            if (i != parameters.size() - 1) {
                result += ", ";
            }
        }
        result += ") {\n";
        result += body->toString();
        result += "\n}";

        return result;
    }
};

}